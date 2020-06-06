#ifndef JFG_CONTAINERS_H
#define JFG_CONTAINERS_H

#include "prelude.h"

template <typename T>
struct Slice
{
	T  *base;
	u32 len;

	operator bool() { return len; }
	T& operator[](u32 idx) { ASSERT(idx < len); return base[idx]; }

	Slice(T* base, u32 len) : base(base), len(len) { }
};

template <typename T, u32 size>
struct Stack
{
	u32 top;
	T   items[size];

	operator bool() { return top; }

	void reset()      { top = 0; }
	void push(T item) { ASSERT(top < size); items[top++] = item; }
	T    peek()       { ASSERT(top); return items[top - 1]; }
	T    pop()        { ASSERT(top); return items[--top]; }
};

template <typename T, u32 size>
struct Max_Length_Array
{
	u32 len;
	T   items[size];

	operator bool()     { return len; }
	operator Slice<T>() { return Slice<T>(items, len); }
	T& operator[](u32 idx) { ASSERT(idx < len); return items[idx]; }

	void reset()        { len = 0; }
	void append(T item) { ASSERT(len < size); items[len++] = item; }
	T    pop()          { ASSERT(len); return items[--len]; }
	Slice<T> slice(u32 start, u32 length)
	{
		ASSERT(start + length < len);
		return Slice<T>(&items[start], length);
	}
};

#endif
