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

static inline f32 lerp(f32 start, f32 end, f32 weight) {
	return start + (end - start) * weight;
}

static inline f32 clamp(f32 low, f32 high, f32 val) {
	return min(max(low, val), high);
}

#endif
