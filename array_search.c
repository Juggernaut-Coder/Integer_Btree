#include "array_search.h"
#include <stdio.h>
/**
 *                       (high - low) * (KeyToLookUp - List[low])               
 * interpolate = low + -------------------------------------------
 *                            (List[high] - List[low])
*/
static inline double interpolate(int value, size_t left, size_t right, int *data) {
    double result = (double)(left) + (((double)(right - left)) * (((double)value) - ((double)data[left])) / ((double)(data[right] - data[left])));
    return result;
}

static inline void update_ib_search_bounds(int value, size_t *interpolation, size_t *left, size_t *right, size_t *mid, int *array) {
    if(value > array[*interpolation]) {
        (*interpolation)++;
        if(value <= array[*interpolation]) {
            *left = *right = *interpolation;
            return;
        }
        *mid = (*interpolation + *right) / 2;
        *left = (value <= array[*mid]) ? ((*right = *mid), (*interpolation + 1)) : (*mid + 1);
    } else {
        size_t lookIdx = *interpolation - 1;
        if(*interpolation == *left || value > array[lookIdx]) {
            *left = *right = *interpolation;
            return;
        }
        *mid = (*interpolation + *left) / 2;
        *right = (value >= array[*mid]) ? ((*left = *mid), lookIdx) : (*mid - 1);
    }
}

bool ibs_isValInArray(int value, int *data, size_t left, size_t data_len, size_t *idx) { 
    /**
     * Assumptions : 
     * 1) data is sorted in ascending order and all the values in data are unique
     * 2) left >=0 and right <= data.size - 1
     * */ 
    if(data_len == 0 || value < data[left]) {
        *idx = left;
        return false;
    }
    size_t mid, right = data_len - 1;
    if(value > data[right]) {
        *idx = data_len;
        return false;
    }
    while(left < right) {
        *idx = (size_t)interpolate(value, left, right, data);
        update_ib_search_bounds(value, idx, &left, &right, &mid, data);
    }
    return (value == data[left]);
}

bool sq_isValInArray(int value, int *data, size_t left, size_t data_len, size_t *idx) {
    *idx = left;
    while(*idx < data_len && value > data[*idx]){
        (*idx)++;
    }
    if(*idx >= data_len) {
        return false;
    } 
    return (data[*idx] == value);
}
