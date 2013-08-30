/** string_utils.c
 *
 *  Created on: Jul 6, 2013
 *      Author: mgrosvenor
 */

#include "string_utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../types/types.h"



//Make a new m6_string on the stack, no free is necessary when done with it, but don,t pass it up a function call
m6_str m6_str_new(char* s,i64 size, m6_bool stack)
{
    m6_str result;
    result.mlen = MIN(size + 1,getpagesize()); //Put a sensible bound on the size of these things
    result.mlen *= 2; //Grow this up a little bit so we don't have to alloc too much
    result.slen = strnlen(s,size);
    if(unlikely(result.slen > result.mlen - 1)){
        result.slen = result.mlen - 1;
    }

    if(stack){
        result.cstr = (char*)alloca(result.mlen);
    }
    else{
        result.cstr = (char*)malloc(result.mlen);
    }
    if(!result.cstr){
        fprintf(stderr,"Error: Could not allocate memory for m6_string\n");
        const m6_str null = M6_STR_NULL;
        result = null;
        return result;
    }

    result.cstr[result.mlen -1] = '\0'; //Always null terminate the whole thing
    memcpy(result.cstr,s,result.slen);//Also null terminate the actual string.
    result.cstr[result.slen] = '\0';

    result.is_const = stack;

    return result;
}

//We can't always set the string length, so sometimes it is unset, this sanitizes it before use
//In general, m6_str should be passed by value, this is one of the few exceptions, but it is a static local
static void m6_str_sanitize(m6_str* rhs)
{
    if(rhs){
        rhs->slen = rhs->slen >= 0 ? rhs->slen : strlen(rhs->cstr);
        rhs->mlen = rhs->mlen >= 0 ? rhs->mlen : rhs->slen + 1; //Include the null terminator
        //TODO Add some other sanity checks here
    }
    else{
        frpintf(stderr, "Error: Null m6_string passed to %s\n", __FUNCTION__);
    }

}


i64 m6_str_resize(m6_str* rhs,i64 newsize)
{

    m6_str_sanitize(rhs);

    if(rhs->is_const != 0){
        return -1; //Cannot resize a constant string or a string with unknown constantnes
    }


    rhs->mlen = MIN(newsize + 1,getpagesize()); //Put a sensible bound on the size of these things
    rhs->mlen *= 2; //Grow this up a little bit so we don't have to alloc too much
    if(unlikely(rhs->slen > rhs.mlen - 1)){
        rhs.slen = rhs.mlen - 1;
    }

    rhs->cstr = realloc(rhs->cstr,rhs->mlen); //Put a little bit of extra memory aside
    if(!rhs->cstr){
        fprintf(stderr, "Error:Could not resize m6_string\n");
        const m6_str null = M6_STR_NULL;
        *rhs = null;
        return -1;
    }

    rhs->cstr[rhs->mlen -1] = '\0'; //Always null terminate the whole thing
    rhs->cstr[rhs->slen] = '\0';

    return 0;
}





//Concatenate two strings and return a string on the stack (no need to free it)
m6_str m6_str_cat(m6_str lhs, m6_str rhs, m6_bool stack)
{
    m6_str_sanitize(&lhs);
    m6_str_sanitize(&rhs);
    const i64 result_size = lhs.slen + rhs.slen + 1;

    m6_str* result = NULL;

    //See if we can use the lhs
    if(lhs.mlen - lhs.slen >= result_size){ //There is enough memory already
        result = &lhs;
    }
    else{
        if(!m6_str_resize(&lhs, result_size)){
            //Woot resized!
            result = &lhs;
        }
        //lame couldn't resize the LHS
    }


    //Can simply cat, RHS onto the back of LHS at this point.
    if(result){
        memcpy(&lhs->cstr[lhs->slen], rhs->cstr, rhs->slen);
        lhs->cstr[result_size] = '\0'; //Make sure we are null terminated
        lhs->cstr[lhs->mlen] = '\0'; //Make sure the whole memory is also null term'd
        //Woot! All done.
        return lhs;
    }


    if(rhs.mlen - rhs.slen >= result_size){ //There is enough memory already
        result = &rhs;
    }
    else{
        if(!m6_str_resize(&rhs, result_size)){
            //Woot resized!
            result = &rhs;
        }
        //lame couldn't resize the RHS too!
    }

    //This is a little more tricky, need to more things around
    if(result){
        memmove(&rhs->cstr[lhs->slen], rhs->cstr, rhs->slen);
        memcpy(&rhs->cstr[0], lhs->cstr, lhs->slen);
        rhs->cstr[result_size] = '\0'; //Make sure we are null terminated
        rhs->cstr[rhs->mlen] = '\0'; //Make sure the whole memory is also null term'd
        //Woot! All done.
        return rhs;
    }

    //If we've got to this point, we need to alloc some new memory for all of this.
    m6_str result_new = m6_str_new(lhs->cstr,result_size,stack);
    //now recurse! Since we have a new LHS allocated enough memory for the rest, this should simply fall into the lhs case above
    return m6_str_cat(result_new,rhs,stack);

}


static void m6_str_free_(m6_str* s){
    if(s->is_const == 0 && s->is_stack == 0){
        free(s->cstr);

        s->cstr = NULL;
        s->mlen = 0;
        s->slen = -1;
    }
}


m6_str m6_str_free(m6_str s){
   m6_str_free(s);
   return s;
}

