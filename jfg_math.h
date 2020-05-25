#ifndef JFG_MATH_H
#define JFG_MATH_H

#include <math.h>

#define MAKE_MIN_MAX_FUNCTIONS(Type) \
	static inline Type min_##Type(Type a, Type b) \
	{ \
		return a < b ? a : b; \
	} \
	static inline Type max_##Type(Type a, Type b) \
	{ \
		return a > b ? a : b; \
	}

MAKE_MIN_MAX_FUNCTIONS(u32)

#undef MAKE_MIN_MAX_FUNCTIONS

#endif
