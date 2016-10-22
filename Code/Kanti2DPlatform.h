#ifndef KANTI2D_PLATFORM

#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <float.h>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef float real32;
typedef double real64;

typedef int32 bool32;

typedef size_t memory_index;

typedef intptr_t intptr;
typedef uintptr_t uintptr;

#define TRUE 1
#define FALSE 0

class Vector2;

// class Vector3;

// class Vector4;

#define Real32Maximum FLT_MAX
#define Real32Minimum -FLT_MAX

#ifndef k_internal
#define k_internal static
#endif
#define local_persist static
#define global_variable static

#define Pi32 3.14159265359f
#define Tau32 6.28318530717958647692f

#if KANTI2D_SLOW
#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#else
#define Assert(Expression)
#endif

#define KANTI_PLATFORM
#endif
