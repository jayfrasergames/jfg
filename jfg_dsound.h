#ifndef JFG_DSOUND_H
#define JFG_DSOUND_H

#include "prelude.h"

// XXX - we should do this
// maybe make a jfg_windows.h file?
// #include <windows.h>

// LPGUID
// LPDIRECTSOUND
// typedef void* LPVOID;
// typedef char* LPCSTR;
// WINAPI

struct DSound_GUID
{
	u32 data1;
	u16 data2;
	u16 data3;
	u8  data4[8];
};

#ifndef JFG_HEADER_ONLY
#define DSOUND_DEF_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	extern "C" DSOUND_GUID name = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } };
#else
#define DSOUND_DEF_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	extern "C" DSOUND_GUID name;
#endif

struct IDirectSound;
typedef IDirectSound *LPDIRECTSOUND;

typedef BOOL (CALLBACK *LPDSENUMCALLBACKA)(LPGUID, LPCSTR, LPCSTR, LPVOID);

#define DSOUND_FUNCTIONS \
	DSOUND_FUNCTION(HRESULT, DirectSoundEnumerateA, LPDSENUMCALLBACKA lpDSEnumCallback, LPVOID context) \
	DSOUND_FUNCTION(HRESULT, DirectSoundCreate, LPGUID lpGuid, LPDIRECTSOUND* ppDS, void* unused)

#define DSOUND_FUNCTION(return_type, name, ...) extern return_type (WINAPI *name)(__VA_ARGS__);
DSOUND_FUNCTIONS
#undef DSOUND_FUNCTION

u8 dsound_try_load();

#ifndef JFG_HEADER_ONLY

#define DSOUND_FUNCTION(return_type, name, ...) return_type (WINAPI *name)(__VA_ARGS__) = NULL;
DSOUND_FUNCTIONS
#undef DSOUND_FUNCTION

u8 dsound_try_load()
{
	HMODULE dsound_library = LoadLibraryA("dsound.dll");
	if (!dsound_library) {
		return 0;
	}
#define DSOUND_FUNCTION(return_type, name, ...) \
		name = (return_type (WINAPI *)(__VA_ARGS__))GetProcAddress(dsound_library, #name);
	DSOUND_FUNCTIONS
#undef DSOUND_FUNCTION
	return 1;
}

#endif

#endif
