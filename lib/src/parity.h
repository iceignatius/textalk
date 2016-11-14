/*
 * Parity utility.
 */
#ifndef _PARITY_H_
#define _PARITY_H_

#include <stddef.h>
#include <stdbool.h>
#include <gen/bits.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline
char parity_ch_add_odd(char ch)
{
    return ( bits_count_bits_8( ch & 0x7F ) & 0x01 )?( ch ):( ch | 0x80 );
}

static inline
char parity_ch_add_even(char ch)
{
    return ( bits_count_bits_8( ch & 0x7F ) & 0x01 )?( ch | 0x80 ):( ch );
}

static inline
char parity_ch_add(char ch, int odd_even)
{
    if( !odd_even )
        return ch;
    if( odd_even & 0x01 )
        return parity_ch_add_odd(ch);
    else
        return parity_ch_add_even(ch);
}

static inline
char parity_ch_remove(char ch)
{
    return ch & 0x7F;
}

static inline
void parity_arr_add_odd(char *arr, size_t len)
{
    for(; len; --len, ++arr)
        *arr = parity_ch_add_odd(*arr);
}

static inline
void parity_arr_add_even(char *arr, size_t len)
{
    for(; len; --len, ++arr)
        *arr = parity_ch_add_even(*arr);
}

static inline
void parity_arr_add(char *arr, size_t len, int odd_even)
{
    if( !odd_even )
        (void)0;  // Nothing to do.
    else if( odd_even & 0x01 )
        return parity_arr_add_odd(arr, len);
    else
        return parity_arr_add_even(arr, len);
}

static inline
void parity_arr_remove(char *arr, size_t len, int odd_even)
{
    if( odd_even )
    {
        for(; len; --len, ++arr)
            *arr = parity_ch_remove(*arr);
    }
}

static inline
bool parity_arr_check_odd(const char *arr, size_t len)
{
    for(; len; --len, ++arr)
    {
        if( !( bits_count_bits_8(*arr) & 0x01 ) )
            return false;
    }

    return true;
}

static inline
bool parity_arr_check_even(const char *arr, size_t len)
{
    for(; len; --len, ++arr)
    {
        if( bits_count_bits_8(*arr) & 0x01 )
            return false;
    }

    return true;
}

static inline
bool parity_arr_check(const char *arr, size_t len, int odd_even)
{
    if( !odd_even )
        return true;
    else if( odd_even & 0x01 )
        return parity_arr_check_odd(arr, len);
    else
        return parity_arr_check_even(arr, len);
}

#ifdef __cplusplus
}  // extern "C"
#endif

#endif
