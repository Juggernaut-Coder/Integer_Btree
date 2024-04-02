#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

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
        lp_cnt++;
    }
    return (value == data[left]);
}


int main(void) {
    int array1[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55};
    int array2[] = {1,10,15,30,400,401,402,600,620,640,650,700,701,702,705,2000,2005,3000,3200,3400,3500,3600,6000,6200,6500,6700,6800,6801,6803,8000,9001,9010,9100,9300,9500,9601,9602,9802,9900};
    int array3[] = {343, 1540, 1562, 2135, 2275, 2559, 3780, 4440, 4946, 6110, 6200, 7040, 7290, 7541, 7881, 9269};
    printf(
        "Arrays available:\n"
        "1)array1[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55}\n"
        "2)array2[] = {1,10,15,30,400,401,402,600,620,640,650,700,701,702,705,2000,2005,3000,3200,3400,3500,3600,6000,6200,6500,6700,6800,6801,6803,8000,9001,9010,9100,9300,9500,9601,9602,9802,9900}\n"
        "3)array3[] = {343, 1540, 1562, 2135, 2275, 2559, 3780, 4440, 4946, 6110, 6200, 7040, 7290, 7541, 7881, 9269};\n"
    );
    int number;
    clock_t start, end;
    double cpu_time_used;
    size_t array1_len = sizeof(array1) / sizeof(array1[0]);
    size_t array2_len = sizeof(array2) / sizeof(array2[0]);
    size_t array3_len = sizeof(array3) / sizeof(array3[0]);
    size_t idx;
    // ibsearch
    while(1) {
        printf("Enter array number:");
        scanf("%d", &number);
        int *p;
        size_t len;
        switch (number)
        {
        case 1:
            p = array1;
            len = array1_len;
            break;
        case 2:
            p = array2;
            len = array2_len;
            break;
        case 3:
            p = array3;
            len = array3_len;
            break;
        default:
            p = NULL;
            len = 0;
            exit(0);
            break;
        }
        printf("Enter a key to find in array %d: ", number);
        scanf("%d", &number);
        printf("You entered: %d\n", number);

        start = clock();
        if(ibs_isValInArray(number, p, 0, len, &idx)) {
            printf("Found at idx: ");
        } else {
            printf("Not found. idx is at :");
        }
        printf("%zu\n", idx);
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("Time taken for IBS in array: %f seconds\n", cpu_time_used);
    }
    return 0;
}