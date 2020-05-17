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

struct v2
{
	union { f32 x, w; };
	union { f32 y, h; };
};

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

struct v2_u32
{
	union { u32 x; u32 w; };
	union { u32 y; u32 h; };
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
