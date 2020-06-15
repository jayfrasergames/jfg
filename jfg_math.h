#ifndef JFG_MATH_H
#define JFG_MATH_H

#include "prelude.h"
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

static inline v2 lerp(v2 start, v2 end, f32 weight) {
	return start + (end - start) * weight;
}

static inline f32 clamp(f32 low, f32 high, f32 val) {
	return min(max(low, val), high);
}

struct m2
{
	union {
		struct {
			f32 _00, _10, _01, _11;
		};
		f32 elements[4];
		v2 rows[2];
	};

	static m2 rotation(f32 theta)
	{
		f32 c = cosf(theta), s = sinf(theta);
		m2 m;
		m._00 = c; m._10 = -s;
		m._01 = s; m._11 =  c;
		return m;
	}
};

v2 operator*(m2 m, v2 v)
{
	return V2_f32(m._00 * v.x + m._10 * v.y, m._01 * v.x + m._11 * v.y);
}

#endif
