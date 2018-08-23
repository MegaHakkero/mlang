#pragma once

#include <sys/stat.h>
#include <stdint.h>
#include <wchar.h>

typedef void     U0;
typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef int8_t   I8;
typedef int16_t  I16;
typedef int32_t  I32;
typedef int64_t  I64;

typedef float    F32;
typedef double   F64;

typedef char     C8;
typedef wchar_t  C16;

typedef struct stat STAT;

// unsigned
#define  U8_WIDTH 8
#define U16_WIDTH 16
#define U32_WIDTH 32
#define U64_WIDTH 64

#define  U8_BYTES 1
#define U16_BYTES 2
#define U32_BYTES 4
#define U64_BYTES 8

// signed
#define  I8_WIDTH 8
#define I16_WIDTH 16
#define I32_WIDTH 32
#define I64_WIDTH 64

#define  I8_BYTES 1
#define I16_BYTES 2
#define I32_BYTES 4
#define I64_BYTES 8

// floating
#define F32_WIDTH 32
#define F64_WIDTH 64

#define F32_BYTES 4
#define F64_BYTES 8

// characters
#define  C8_WIDTH 8
#define C16_WIDTH 16

#define  C8_BYTES 1
#define C16_BYTES 2

#define PTR_WIDTH() (sizeof(U0 *) * CHAR_BIT)
#define PTR_BYTES() (sizeof(U0 *))
