#ifndef JFG_PRELUDE_H
#define JFG_PRELUDE_H

#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float  f32;
typedef double f64;

#define V2(Type) \
	struct v2_##Type \
	{ \
		union { Type x, w; }; \
		union { Type y, h; }; \
	};

V2(f32); V2(u8); V2(u16); V2(u32); V2(u64);
V2(f64); V2(i8); V2(i16); V2(i32); V2(i64);

#undef V2

typedef v2_f32 v2;

struct v3
{
	f32 x, y, z;
};

struct v4
{
	union { f32 x, r; };
	union { f32 y, g; };
	union { f32 z, b; };
	union { f32 w, a; };
};


struct m3x2
{
	union
	{
		v2 cols[3];
		f32 elems[6];
	};
};

#define ARRAY_SIZE(xs) (sizeof(xs)/sizeof(xs[0]))
#define OFFSET_OF(struct_type, member) ((size_t)(&((struct_type*)0)->member))
#define STATIC_ASSERT(COND, MSG) typedef u8 static_assertion_##MSG[(COND) ? 1 : -1]

#ifdef LIBRARY
	#define LIBRARY_EXPORT extern "C" __declspec(dllexport)
#else
	#define LIBRARY_EXPORT
#endif

#endif
