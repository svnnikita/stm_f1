#include <inttypes.h>
#include "digitalFilter_operations/digitalFilter_operations.hpp"

int32_t sum_p(int32_t * sum_num1, int32_t * sum_num2) {      // сложение чисел
    return (*sum_num1) + (*sum_num2);    
}

int32_t mult_p(int32_t * mult_num1, int32_t * mult_num2) {   // умножение чисел
    return (*mult_num1) * (*mult_num2);    
}