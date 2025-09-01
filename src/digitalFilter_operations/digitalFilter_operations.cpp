#include <inttypes.h>
#include "digitalFilter_operations/digitalFilter_operations.hpp"

// Вычисляем один отсчет фильтра
void Calculating_Counts( out_values *fout_values, 
                            coefs *fcoefs, 
                            int64_t* X_in, 
                            FilterBuffer *zbuff_axbx1,
                            FilterBuffer *zbuff_axbx2,
                            FilterBuffer *zbuff_axbx3,
                            FilterBuffer *zbuff_axbx4 ) {

    fout_values->b0_out = fcoefs->b0;                               // Умножаем вход на B0
    fout_values->y_out = fout_values->b0_out + (*zbuff_axbx1->out); // Результат проходит на выход
    
    // Вычисляем задержку a4b4 
    fout_values->a4_out = (fout_values->y_out * fcoefs->a4) / *X_in;  // a4 * Y
    fout_values->b4_out = fcoefs->b4;                     // b4 * X
    *(zbuff_axbx4->in) = fout_values->b4_out + fout_values->a4_out; // b4 * X + a4 * Y  

    // Вычисляем задержку a3b3 
    fout_values->a3_out = (fout_values->y_out * fcoefs->a3) / *X_in;    // a3 * Y
    fout_values->b3_out = fcoefs->b3;                 // b3 * X
    *(zbuff_axbx3->in) = fout_values->a3_out + fout_values->b3_out + *(zbuff_axbx4->out);

    // Вычисляем задержку a2b2 
    fout_values->a2_out = (fout_values->y_out * fcoefs->a2) / *X_in;    // a2 * Y
    fout_values->b2_out = fcoefs->b2;                 // b2 * X
    *(zbuff_axbx2->in) = fout_values->a2_out + fout_values->b2_out + *(zbuff_axbx3->out);

    // Вычисляем задержку a1b1 
    fout_values->a1_out = (fout_values->y_out * fcoefs->a1) / *X_in;    // a1 * Y
    fout_values->b1_out = fcoefs->b1;                 // b1 * X
    *(zbuff_axbx1->in) = fout_values->a1_out + fout_values->b1_out + *(zbuff_axbx2->out);
}

// Циклическое перемещение указателей буфера
void Moving_Pointers( FilterBuffer *zbuff) 
{
    // Обмен указателей in и out
    int64_t *temp = zbuff->in;
    zbuff->in = zbuff->out;
    zbuff->out = temp;
}
