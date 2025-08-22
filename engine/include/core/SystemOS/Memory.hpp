#ifndef __ENGINE_MEMORY_HPP__
#define __ENGINE_MEMORY_HPP__

#include "../Errors/ErrorMacros.hpp"
#include "../Templates/SafeRefcount.hpp"

#include <new>
#include <type_traits>

#define DEBUG_ENABLED

class Memory {

public:
    /**
     * Alignment:  ↓ max_align_t        ↓ uint64_t          ↓ max_align_t
     *             ┌─────────────────┬──┬────────────────┬──┬───────────...
     *             │ uint64_t        │░░│ uint64_t       │░░│ T[]
     *             │ alloc size      │░░│ element count  │░░│ data
     *             └─────────────────┴──┴────────────────┴──┴───────────...
     * Offset:     ↑ SIZE_OFFSET        ↑ ELEMENT_OFFSET    ↑ DATA_OFFSET
     */

    static constexpr size_t SIZE_OFFSET{0};

    static constexpr size_t ELEMENT_OFFSET{((SIZE_OFFSET + sizeof(uint64_t)) %
                                                alignof(uint64_t) ==
                                            0)
                                               ? (SIZE_OFFSET + sizeof(uint64_t))
                                               : ((SIZE_OFFSET + sizeof(uint64_t)) + alignof(uint64_t) - ((SIZE_OFFSET + sizeof(uint64_t)) % alignof(uint64_t)))};

    static constexpr size_t DATA_OFFSET{((ELEMENT_OFFSET + sizeof(uint64_t)) %
                                             alignof(max_align_t) ==
                                         0)
                                            ? (ELEMENT_OFFSET + sizeof(uint64_t))
                                            : ((ELEMENT_OFFSET + sizeof(uint64_t)) + alignof(max_align_t) -
                                               ((ELEMENT_OFFSET + sizeof(uint64_t)) % alignof(max_align_t)))};

    template <bool p_ensureZero = false>
    static void *allocStatic(size_t p_bytes, bool p_padAlign = false);
    _FORCE_INLINE_ static void *allocateStaticZeroed(size_t p_bytes, bool p_padAlign = false) {
        return allocStatic<true>(p_bytes, p_padAlign);
    }

    static void *reallocStatic(void *p_memory, size_t p_bytes, bool p_padAlign = false);
    static void freeStatic(void *p_ptr, bool p_padAlign = false);

    /**
     *                              ↓ return value of alloc_aligned_static
     *	┌─────────────────┬─────────┬─────────┬──────────────────┐
     *	│ padding (up to  │ uint32_t│ void*   │ padding (up to   │
     *	│ p_alignment - 1)│ offset  │ p_bytes │ p_alignment - 1) │
     *	└─────────────────┴─────────┴─────────┴──────────────────┘
     *
     * alloc_aligned_static will allocate p_bytes + p_alignment - 1 + sizeof(uint32_t) and
     * then offset the pointer until alignment is satisfied.
     *
     * This offset is stored before the start of the returned ptr so we can retrieve the original/real
     * start of the ptr in order to free it.
     *
     * The rest is wasted as padding in the beginning and end of the ptr. The sum of padding at
     * both start and end of the block must add exactly to p_alignment - 1.
     *
     * p_alignment MUST be a power of 2.
     */
    static void *allocAlignedStatic(size_t p_bytes, size_t p_alignment);
    static void *reallocAlignedStatic(void *p_memory, size_t p_bytes, size_t p_prevBytes,
                                      size_t p_alignment);

    /**
     * Pass the ptr returned by allocAlignedStatic to free it.
     * e.g.
     * void* data = reallocAlignedStatic(bytes, 16);
     * freeAlignedStatic(data);
     */
    static void freeAlignedStatic(void *p_memory);

    static uint64_t getMemoryAvailable();
    static uint64_t getMemoryUsage();
    static uint64_t getMemoryMaxUsage();

private:
#ifdef DEBUG_ENABLED
    static SafeNumeric<uint64_t> m_memoryUsage;
    static SafeNumeric<uint64_t> m_maxUsage;
#endif
};

class DefaultAllocator {
public:
    _FORCE_INLINE_ static void *alloc(size_t p_memory) {
        return Memory::allocStatic(p_memory, false);
    }

    _FORCE_INLINE_ static void free(void *p_ptr) {
        Memory::freeStatic(p_ptr, false);
    }
};

/** Operator new that takes a description and uses MemoryStaticPool. */
void *operator new(size_t p_size, const char *p_description);

/** Operator new that takes a description and uses MemoryStaticPool */
void *operator new(size_t p_size, void *(*p_allocFunction)(size_t p_size));

/** Operator new that takes a description and uses a pointer to the preallocated memory. */
void *operator new(size_t p_size, void *p_pointer, size_t check, const char *p_description);

#ifdef _MSC_VER
/**
 * When compiling with VC++ 2017, the above declarations of placement new generate many irrelevant warnings (C4291).
 * The purpose of the following definitions is to muffle these warnings, not to provide a usable implementation of placement delete.
 */
void operator delete(void *p_memory, const char *p_description);
void operator delete(void *p_memory, void *(*p_allocfunc)(size_t p_size));
void operator delete(void *p_memory, void *p_pointer, size_t check, const char *p_description);
#endif

#define memoryAlloc(m_size) Memory::allocStatic(m_size)
#define memoryAllocZeroed(m_size) Memory::allocStaticZeroed(m_size)
#define memoryRealloc(m_memory, m_size) Memory::reallocStatic(m_memory, m_size)
#define memoryFree(m_memory) Memory::freeStatic(m_memory)

_ALWAYS_INLINE_ void postinitializeHandler(void *) {}

template <typename T>
_ALWAYS_INLINE_ T *postInitialize(T *p_obj) {
    postinitializeHandler(p_obj);

    return p_obj;
}

#define memoryNew(m_class) postInitialize(::new ("") m_class)

#define memoryNewAllocator(m_class, m_allocator) postInitialize(::new (m_allocator::alloc) m_class)
#define memoryNewPlacement(m_placement, m_class) postInitialize(::new (m_placement) m_class)

_ALWAYS_INLINE_ bool predeleteHandler(void *) {
    return true;
}

template <typename T>
void memoryDelete(T *p_class) {
    if (!predeleteHandler(p_class)) {
        return;
    }

    if constexpr (!std::is_trivially_destructible_v<T>) {
        p_class->~T();
    }

    Memory::freeStatic(p_class, false);
}

template <typename T, typename A>
void memoryDeleteAllocator(T *p_class) {
    if (!predeleteHandler(p_class)) {
        return;
    }

    if constexpr (!std::is_trivially_destructible_v<T>) {
        p_class->~T();
    }

    A::free(p_class);
}

#define memoryDeleteNotnull(m_v) { \
        if (m_v) {                 \
            memoryDelete(m_v);     \
        }                          \
    }

#define memoryNewArray(m_class, m_count) memoryNewArrayTemplate<m_class>(m_count)

_FORCE_INLINE_ uint64_t *getElementCountPtr(uint8_t *p_ptr) {
    return (uint64_t *)(p_ptr - Memory::DATA_OFFSET + Memory::ELEMENT_OFFSET);
}

template <typename T>
T *memoryNewArrayTemplate(size_t p_elements) {
    if (p_elements == 0) {
        return nullptr;
    }

    /** overloading operator new[] cannot be done , because it may not return the real allocated address (it may pad the 'element count' before the actual array). Because of that, it must be done by hand. This is the
    same strategy used by std::vector, and the Vector class, so it should be safe.*/

    size_t len = sizeof(T) * p_elements;
    uint8_t *memory = (uint8_t *)Memory::allocStatic(len, true);
    T *failptr = nullptr;
    ERR_FAIL_NULL_V(memory, failptr);

    uint64_t *_elem_count_ptr = getElementCountPtr(memory);
    *(_elem_count_ptr) = p_elements;

    if constexpr (!std::is_trivially_constructible_v<T>) {
        T *elems = (T *)memory;

        /* call operator new */
        for (size_t i = 0; i < p_elements; i++) {
            ::new (&elems[i]) T;
        }
    }

    return (T *)memory;
}

// Fast alternative to a loop constructor pattern.
template <typename T>
_FORCE_INLINE_ void memoryNewArrayPlacement(T *p_start, size_t p_num) {
    if constexpr (is_zero_constructible_v<T>) {
        // Can optimize with memset.
        memset(static_cast<void *>(p_start), 0, p_num * sizeof(T));
    } else {
        // Need to use a for loop.
        for (size_t i = 0; i < p_num; i++) {
            memoryNewPlacement(p_start + i, T());
        }
    }
}

/**
 * Wonders of having own array functions, you can actually check the length of
 * an allocated-with memnew_arr() array
 */

template <typename T>
size_t memoryArrayLength(const T *p_class) {
    uint8_t *ptr = (uint8_t *)p_class;
    uint64_t *_elem_count_ptr = getElementCountPtr(ptr);
    return *(_elem_count_ptr);
}

template <typename T>
void memdelete_arr(T *p_class) {
    uint8_t *ptr = (uint8_t *)p_class;

    if constexpr (!std::is_trivially_destructible_v<T>) {
        uint64_t *_elem_count_ptr = getElementCountPtr(ptr);
        uint64_t elem_count = *(_elem_count_ptr);

        for (uint64_t i = 0; i < elem_count; i++) {
            p_class[i].~T();
        }
    }

    Memory::freeStatic(ptr, true);
}

struct _GlobalNil {
    int color = 1;
    _GlobalNil *right = nullptr;
    _GlobalNil *left = nullptr;
    _GlobalNil *parent = nullptr;

    _GlobalNil();
};

struct _GlobalNilClass {
    static _GlobalNil _nil;
};

template <typename T>
class DefaultTypedAllocator {
public:
    template <typename... Args>
    _FORCE_INLINE_ T *newAllocation(const Args &&...p_args) {
        return memoryNew(T(p_args...));
    }

    _FORCE_INLINE_ void deleteAllocation(T *p_allocation) {
        memoryDelete(p_allocation);
    }
};

#endif
