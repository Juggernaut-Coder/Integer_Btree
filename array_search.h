#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

bool ibs_isValInArray(int value, int *data, size_t left, size_t data_len, size_t *idx);
bool sq_isValInArray(int value, int *data, size_t left, size_t data_len, size_t *idx);