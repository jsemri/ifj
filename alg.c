#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include "string.h"
#include "alg.h"


/**
 * \brief Functions sorts strings.
 *
 * @param   src    Pointer to string.
 * @param   dest   Pointer to string, where sorted string will be stored.
 *
 * @return  HEAPSORT_OK     If success.
 * @return  HEAPSORT_FAIL   If duplication of new new string failed.
 */
int heap_sort(T_string *src, T_string *dest)
{

	if (str_copy(dest, src) != 1){
        return HEAPSORT_FAIL;
    }

	int n = dest->length - 1;
	int left = n / 2;
	int right = n;
	char tmp;
    //establishment of heap   
    for (int i = left; i >= 0; i--) {
        sift_down(dest, i, right);
    }
    //heap-sort
    for (right = n; right >= 1; right--) {
        tmp = dest->buf[0];
        dest->buf[0] = dest->buf[right];
        dest->buf[right] = tmp;
        sift_down(dest, 0, right - 1);
    }

    return HEAPSORT_OK;
}

/**
 * \brief Function for heapsort, reeastablishment of heap.
 * @param   string   Pointer to string.
 * @param   left     Index of the most left node.
 * @param   right    Index of the most righ node.
 */
void sift_down(T_string *string, int left, int right)
{   
    int i = left;
    int j = 2 * i; //left son index
    char tmp = string->buf[i];
    bool cont = j <= right;
    
    while (cont) {
        if (j < right)
            if (string->buf[j] < string->buf[j + 1]) {
                j = j + 1;
            } // if
        if (tmp >= string->buf[j])
            cont = false;
        else {
            string->buf[i] = string->buf[j];
            i = j;
            j = 2 * i;
            cont = j <= right;
        }
    }
    string->buf[i] = tmp;
}

/**
 * \brief Function finds substring in buf using Knuth-Morris-Pratt algorithm
 *		  and returns its position.
 *        If substring is not found, function returns -1.
 *
 * @param   buf    Pointer to buf.
 * @param   pattern   Pointer to substring.
 *
 * @return  Index of first occurrence substring in buf.
 */
int find_kmp(T_string *string, T_string *pattern)
{
    unsigned k;
    int r;
    int fail_vector[pattern->length];
    
    fail_vector[0] = -1;
    for (k = 1; k < pattern->length; k++) {
        r = fail_vector[k-1];
        while ((r > -1) && (pattern->buf[r] != pattern->buf[k-1]))
            r = fail_vector[r];
        fail_vector[k] = r + 1;
    }

    unsigned string_index = 0;
    unsigned pattern_index = 0;

    while ((string_index < string->length) && (pattern_index < pattern->length)) {
        if ((pattern_index == 0) || (string->buf[string_index] == pattern->buf[pattern_index])) {
            string_index++;
            pattern_index++;
        }
        else 
            pattern_index = fail_vector[pattern_index];
    }
    if (pattern_index == pattern->length)
        return string_index - pattern->length;
    else
        return -1;
}