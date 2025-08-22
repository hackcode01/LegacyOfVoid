#ifndef __ENGINE_SAFE_REFCOUNT_HPP__
#define __ENGINE_SAFE_REFCOUNT_HPP__

#include "../Typedefs.hpp"

#define DEV_ENABLED

#ifdef DEV_ENABLED
#include "../Errors/ErrorMacros.hpp"
#endif

#include <atomic>

/** IWYU pragma: keep - Used in macro. */
#include <type_traits>

/**
 * Design goals for these classes:
 * - No automatic conversions or arithmetic operators,
 *   to keep explicit the use of atomics everywhere.
 * - Using acquire-release semantics, even to set the first value.
 *   The first value may be set relaxedly in many cases, but adding the distinction
 *   between relaxed and unrelaxed operation to the interface would make it needlessly
 *   flexible. There's negligible waste in having release semantics for the initial
 *   value and, as an important benefit, you can be sure the value is properly synchronized
 *   even with threads that are already running.
 */

/** These are used in very specific areas of the engine where it's critical that these guarantees are held. */
#define SAFE_NUMERIC_TYPE_PUN_GUARANTEES(m_type)                          \
    static_assert(sizeof(SafeNumeric<m_type>) == sizeof(m_type));         \
    static_assert(alignof(SafeNumeric<m_type>) == alignof(m_type));       \
    static_assert(std::is_trivially_destructible_v<std::atomic<m_type>>); \

#define SAFE_FLAG_TYPE_PUN_GUARANTEES                 \
    static_assert(sizeof(SafeFlag) == sizeof(bool));  \
    static_assert(alignof(SafeFlag) == alignof(bool));


template <typename T>
class SafeNumeric {

public:
    _ALWAYS_INLINE_ void set(T p_value) {
        value.store(p_value, std::memory_order_release);
    }

    _ALWAYS_INLINE_ T get() const {
        return m_value.load(std::memory_order_acquire);
    }

    _ALWAYS_INLINE_ T increment() {
        return m_value.fetch_add(1, std::memory_order_acq_rel) + 1;
    }

    /** Returns the original value instead of the new one. */
    _ALWAYS_INLINE_ T postIncrement() {
    	return m_value.fetch_add(1, std::memory_order_acq_rel);
    }

    _ALWAYS_INLINE_ T decrement() {
    	return m_value.fetch_sub(1, std::memory_order_acq_rel) - 1;
    }

    /** Returns the original value instead of the new one. */
    _ALWAYS_INLINE_ T postDecrement() {
    	return m_value.fetch_sub(1, std::memory_order_acq_rel);
    }

    _ALWAYS_INLINE_ T add(T p_value) {
    	return m_value.fetch_add(p_value, std::memory_order_acq_rel) + p_value;
    }

    /** Returns the original value instead of the new one. */
    _ALWAYS_INLINE_ T postAdd(T p_value) {
    	return m_value.fetch_add(p_value, std::memory_order_acq_rel);
    }

    _ALWAYS_INLINE_ T sub(T p_value) {
    	return m_value.fetch_sub(p_value, std::memory_order_acq_rel) - p_value;
    }

    _ALWAYS_INLINE_ T bitOr(T p_value) {
    	return m_value.fetch_or(p_value, std::memory_order_acq_rel);
    }

    _ALWAYS_INLINE_ T bitAnd(T p_value) {
    	return m_value.fetch_and(p_value, std::memory_order_acq_rel);
    }

    _ALWAYS_INLINE_ T bitXor(T p_value) {
    	return m_value.fetch_xor(p_value, std::memory_order_acq_rel);
    }

    /** Returns the original value instead of the new one. */
    _ALWAYS_INLINE_ T postSub(T p_value) {
    	return m_value.fetch_sub(p_value, std::memory_order_acq_rel);
    }

    _ALWAYS_INLINE_ T exchangeIfGreater(T p_value) {
    	while (true) {
    		T tmp = m_value.load(std::memory_order_acquire);
    		if (tmp >= p_value) {
    			/** Already greater, or equal. */
                return tmp;
    		}

    		if (m_value.compare_exchange_weak(tmp, p_value, std::memory_order_acq_rel)) {
    			return p_value;
    		}
    	}
    }

    _ALWAYS_INLINE_ T conditionalIncrement() {
    	while (true) {
    		T c = m_value.load(std::memory_order_acquire);
    		if (c == 0) {
    			return 0;
    		}

    		if (m_value.compare_exchange_weak(c, c + 1, std::memory_order_acq_rel)) {
    			return c + 1;
    		}
    	}
    }

    _ALWAYS_INLINE_ explicit SafeNumeric(T p_value = static_cast<T>(0)) {
    	set(p_value);
    }

private:
    std::atomic<T> m_value;

    static_assert(std::atomic<T>::is_always_lock_free);
};

class SafeFlag {

public:
	_ALWAYS_INLINE_ bool isSet() const {
        return m_flag.load(std::memory_order_acquire);
	}

	_ALWAYS_INLINE_ void set() {
        m_flag.store(true, std::memory_order_release);
	}

	_ALWAYS_INLINE_ void clear() {
        m_flag.store(false, std::memory_order_release);
	}

	_ALWAYS_INLINE_ void setTo(bool p_value) {
        m_flag.store(p_value, std::memory_order_release);
	}

	_ALWAYS_INLINE_ explicit SafeFlag(bool p_value = false) {
        setTo(p_value);
	}

private:
    std::atomic_bool m_flag;

    static_assert(std::atomic_bool::is_always_lock_free);
};

class SafeRefCount {

public:
	_ALWAYS_INLINE_ bool ref() {
        return m_count.conditionalIncrement() != 0;
	}
    
	_ALWAYS_INLINE_ uint32_t refval() {
        return m_count.conditionalIncrement();
	}
    
	_ALWAYS_INLINE_ bool unref() {
    #ifdef DEV_ENABLED
		checkUnrefSafety();
    #endif
		return m_count.decrement() == 0;
	}
    
	_ALWAYS_INLINE_ uint32_t unrefval() {
    #ifdef DEV_ENABLED
		checkUnrefSafety();
    #endif
		return m_count.decrement();
	}

	_ALWAYS_INLINE_ uint32_t get() const {
        return m_count.get();
	}

	_ALWAYS_INLINE_ void init(uint32_t p_value = 1) {
        m_count.set(p_value);
	}

private:
    SafeNumeric<uint32_t> m_count;

#ifdef DEV_ENABLED
	_ALWAYS_INLINE_ void checkUnrefSafety() {
        CRASH_COND_MSG(m_count.get() == 0,
            "Trying to unreference a SafeRefCount which is already zero is wrong and a symptom of it being misused.\n"
            "Upon a SafeRefCount reaching zero any object whose lifetime is tied to it, as well as the ref m_count itself, must be destroyed.\n"
            "Moreover, to guarantee that, no multiple threads should be racing to do the final unreferencing to zero.");
	}
#endif
};

#endif
