#ifndef __ENGINE_ARRAY_HPP__
#define __ENGINE_ARRAY_HPP__

#include "../Typedefs.hpp"
#include "VariantDeepDuplicate.hpp"

#include <climits>
#include <initializer_list>

class Callable;
class StringName;
class Variant;

struct ArrayPrivate;
struct ContainerType;

class Array {

public:
    struct ConstIterator {
	_FORCE_INLINE_ const Variant &operator*() const;
	_FORCE_INLINE_ const Variant *operator->() const;

	_FORCE_INLINE_ ConstIterator &operator++();
	_FORCE_INLINE_ ConstIterator &operator--();

	_FORCE_INLINE_ bool operator==(const ConstIterator &p_other) const { return m_elementPtr == p_other.m_elementPtr; }
	_FORCE_INLINE_ bool operator!=(const ConstIterator &p_other) const { return m_elementPtr != p_other.m_elementPtr; }

	_FORCE_INLINE_ ConstIterator(const Variant *p_element_ptr) :
			m_elementPtr(p_element_ptr) {}
	_FORCE_INLINE_ ConstIterator() {}
	_FORCE_INLINE_ ConstIterator(const ConstIterator &p_other) :
			m_elementPtr(p_other.m_elementPtr) {}

	_FORCE_INLINE_ ConstIterator &operator=(const ConstIterator &p_other) {
		m_elementPtr = p_other.m_elementPtr;
		return *this;
	}

private:
	const Variant *m_elementPtr = nullptr;
};

struct Iterator {
	_FORCE_INLINE_ Variant &operator*() const;
	_FORCE_INLINE_ Variant *operator->() const;

	_FORCE_INLINE_ Iterator &operator++();
	_FORCE_INLINE_ Iterator &operator--();

	_FORCE_INLINE_ bool operator==(const Iterator &p_other) const { return m_elementPtr == p_other.m_elementPtr; }
	_FORCE_INLINE_ bool operator!=(const Iterator &p_other) const { return m_elementPtr != p_other.m_elementPtr; }

	_FORCE_INLINE_ Iterator(Variant *p_element_ptr, Variant *p_read_only = nullptr) :
			m_elementPtr(p_element_ptr), m_readOnly(p_read_only) {}
	_FORCE_INLINE_ Iterator() {}
	_FORCE_INLINE_ Iterator(const Iterator &p_other) :
			m_elementPtr(p_other.m_elementPtr), m_readOnly(p_other.m_readOnly) {}

	_FORCE_INLINE_ Iterator &operator=(const Iterator &p_other) {
		m_elementPtr = p_other.m_elementPtr;
		m_readOnly = p_other.m_readOnly;
		return *this;
	}

	operator ConstIterator() const {
		return ConstIterator(m_elementPtr);
	}

private:
	Variant *m_elementPtr = nullptr;
	Variant *m_readOnly = nullptr;
};

Iterator begin();
Iterator end();

ConstIterator begin() const;
ConstIterator end() const;

Variant &operator[](int_fast32_t p_idx);
const Variant &operator[](int_fast32_t p_idx) const;

void set(int_fast32_t p_idx, const Variant &p_value);
const Variant &get(int_fast32_t p_idx) const;

int_fast32_t size() const;
bool isEmpty() const;
void clear();

bool operator==(const Array &p_array) const;
bool operator!=(const Array &p_array) const;
bool recursiveEqual(const Array &p_array, int_fast32_t recursion_count) const;

uint32_t hash() const;
uint32_t recursiveHash(int_fast32_t recursion_count) const;
void operator=(const Array &p_array);

void assign(const Array &p_array);
void pushBack(const Variant &p_value);

/** for python compatibility. */
_FORCE_INLINE_ void append(const Variant &p_value) { pushBack(p_value); }

void appendArray(const Array &p_array);
Errors resize(int_fast32_t p_new_size);

Errors insert(int_fast32_t p_pos, const Variant &p_value);
void removeAt(int_fast32_t p_pos);
void fill(const Variant &p_value);

Variant front() const;
Variant back() const;
Variant pickRandom() const;

void sort();
void sortCustom(const Callable &p_callable);
void shuffle();
int_fast32_t bsearch(const Variant &p_value, bool p_before = true) const;
int_fast32_t bsearchCustom(const Variant &p_value, const Callable &p_callable, bool p_before = true) const;
void reverse();

int_fast32_t find(const Variant &p_value, int_fast32_t p_from = 0) const;
int_fast32_t findCustom(const Callable &p_callable, int_fast32_t p_from = 0) const;
int_fast32_t rfind(const Variant &p_value, int_fast32_t p_from = -1) const;
int_fast32_t rfindCustom(const Callable &p_callable, int_fast32_t p_from = -1) const;
int_fast32_t count(const Variant &p_value) const;
bool has(const Variant &p_value) const;

void erase(const Variant &p_value);

void pushFront(const Variant &p_value);
Variant popBack();
Variant popFront();
Variant popAt(int_fast32_t p_pos);

Array duplicate(bool p_deep = false) const;
Array duplicateDeep(ResourceDeepDuplicateMode p_deep_subresources_mode = RESOURCE_DEEP_DUPLICATE_INTERNAL) const;
Array recursiveDuplicate(bool p_deep, ResourceDeepDuplicateMode p_deep_subresources_mode, int_fast32_t recursion_count) const;

Array slice(int_fast32_t p_begin, int_fast32_t p_end = INT_MAX, int_fast32_t p_step = 1, bool p_deep = false) const;
Array filter(const Callable &p_callable) const;
Array map(const Callable &p_callable) const;
Variant reduce(const Callable &p_callable, const Variant &p_accum) const;
bool any(const Callable &p_callable) const;
bool all(const Callable &p_callable) const;

bool operator<(const Array &p_array) const;
bool operator<=(const Array &p_array) const;
bool operator>(const Array &p_array) const;
bool operator>=(const Array &p_array) const;

Variant min() const;
Variant max() const;

const void *id() const;

void setTyped(const ContainerType &p_element_type);
void setTyped(uint32_t p_type, const StringName &p_class_name, const Variant &p_script);

bool isTyped() const;
bool isSameTyped(const Array &p_other) const;
bool isSameInstance(const Array &p_other) const;

ContainerType getElementType() const;
uint32_t getTypedBuiltin() const;
StringName getTypedClassName() const;
Variant getTypedScript() const;

void makeReadOnly();
bool isReadOnly() const;
static Array createReadOnly();

Array(const Array &p_base, uint32_t p_type, const StringName &p_class_name, const Variant &p_script);
Array(const Array &p_from);
Array(std::initializer_list<Variant> p_init);
Array();

~Array();

private:
    mutable ArrayPrivate *m_arrayPrivate;

    void ref(const Array &p_from) const;
    void unref() const;
};

#endif
