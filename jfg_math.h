#ifndef JFG_MATH_H
#define JFG_MATH_H

#include <math.h>

// a tad excessive...
#define PI 3.141592653589793238462643383279502884197169399375105820974944

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
