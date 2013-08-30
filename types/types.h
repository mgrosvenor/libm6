/*
 * Copyright  (C) Matthew P. Grosvenor, 2012, All Rights Reserved
 */


#ifndef M6_TYPES_H_
#define M6_TYPES_H_

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>


#include "../deps/libcgen/vector.h"

//Please avoid using char*. M6 strings are a thin wrapper, that support easy concatenation and other functions
typedef struct {
    char* cstr; //Can be NULL or point to a const or non-const memory area
    int is_const; //Can be -1 (unknown), 0 (can call free) or 1 (cannot call free)
    int is_stack; //Can be -1 (unknown), 0 (not stack allocated) or 1 (stack allocated)
    i64 slen; //A valid string len is always -1 (unknown), or < mlen
    i64 mlen; //A valid memory len is always -1 (unknown), or > slen
} m6_str;


//Unsigned types, use these sparingly
typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64; //Try to avoid this unless absolutely necessary. Too many bugs are caused by int/uint mixups

//Signed types, use these sparingly
typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64; //Try to avoid this, use m6_word.


//M6 Library Preferred Types
typedef size_t  m6_machine; //Use this sparingly, only when it really matters that the int size is the same as the machine size
typedef u8      m6_char;
typedef i64     m6_word; //Singed int chosen specifically to avoid underrun and signed/unsigned comparisons
typedef bool    m6_bool; //Give the compiler tons of freedom to decide what to do here
typedef bstring m6_str;
//#define cstr(bstr) ((char*)(bstr->data))

typedef enum {
    M6_NO_TYPE = 0,
    M6_BOOL,
    M6_UINT64,
    M6_INT64,
    M6_STRING,
    M6_DOUBLE,
    M6_BOOLS,     //Vector types
    M6_UINT64S,
    M6_INT64S,
    M6_STRINGS,
    M6_DOUBLES,
} m6_types_e;

m6_word is_vector(m6_word type);





#endif /* M6_TYPES_H_ */
