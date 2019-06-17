#ifndef PPE_TYPES_H
#define PPE_TYPES_H 1

#include <sys/types.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*== References:
 *      https://blog.csdn.net/wyywatdl/article/details/4683762
 *      https://en.m.wikipedia.org/wiki/64-bit_computing
 *
 *      TYPE        LP32    ILP32   LP64    ILP64   LLP64
 *      CHAR        8       8       8       8       8
 *      SHORT       16      16      16      16      16
 *      INT         16      32      32      64      32
 *      LONG        32      32      64      64      32
 *      LONG LONG   64      64      64      64      64
 *      POINTER     32      32      64      64      64
 ==*/

/* ---- Basic Type Definitions ---- */

/* -- Signed Integers -- */

typedef char ppe_int8;
typedef short int ppe_int16;
typedef long long ppe_int64;

#if defined(PPE_DEF_LP32) || defined(PPE_DEF_ILP32) || defined(PPE_DEF_LLP64)

/* LLP64: Microsoft Windows (x86-64 and IA-64) using Visual C++; and MinGW. */

typedef long int ppe_int32;

#elif defined(PPE_DEF_LP64)

/* LP64: Most Unix and Unix-like systems, e.g., Solaris, Linux, BSD, macOS. Windows when using Cygwin; z/OS. */

typedef int ppe_int32;

#elif defined(PPE_DEF_ILP64)

/* No ppe_int32 for the ILP64 model. */

#endif

typedef int ppe_int;

/* -- Unsigned Integers -- */

typedef unsigned char ppe_uint8, ppe_byte;
typedef unsigned short int ppe_uint16, ppe_word;
typedef unsigned long long ppe_uint64, ppe_qword;

#if defined(PPE_DEF_LP32) || defined(PPE_DEF_ILP32) || defined(PPE_DEF_LLP64)

/* LLP64: Microsoft Windows (x86-64 and IA-64) using Visual C++; and MinGW. */

typedef unsigned long int ppe_uint32, ppe_word;

#elif defined(PPE_DEF_LP64)

/* LP64: Most Unix and Unix-like systems, e.g., Solaris, Linux, BSD, macOS. Windows when using Cygwin; z/OS. */

typedef unsigned int ppe_uint32, ppe_word;

#elif defined(PPE_DEF_ILP64)

/* No ppe_int32 for the ILP64 model. */

#endif

typedef unsigned int ppe_uint;

typedef ssize_t ppe_ssize;
typedef size_t ppe_size;
typedef off_t ppe_off;

typedef unsigned char ppe_char;

#if defined(PPE_DEF_C99)

#include <stdbool.h>

typedef bool ppe_bool;
const ppe_bool ppe_false = false;
const ppe_bool ppe_true = true;

#else

typedef enum { ppe_false = 0, ppe_true = 1 } ppe_bool;

#endif

#endif /* PPE_TYPES_H */

