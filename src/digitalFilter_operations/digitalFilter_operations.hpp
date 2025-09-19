#define BUFF_SIZE 2

// Объявление буферов
typedef struct {
    int64_t buffer[BUFF_SIZE];

    // Задержки для каждой пары коэффициенты
    int64_t *in;
    int64_t *out;
} FilterBuffer;

// Хранение промежуточных значений фильтра
typedef struct {
    int64_t a1_out, a2_out, a3_out, a4_out, 
           b0_out, b1_out, b2_out, b3_out, 
           b4_out, y_out;
} out_values;

// Коэффициенты фильтра
typedef struct {
    int64_t a1, a2, a3, a4, 
           b0, b1, b2, b3, b4;
} coefs;

// Вычисляем один отсчет фильтра
void Calculating_Counts( out_values *fout_values, 
                            coefs *fcoefs, 
                            int64_t* X_in, 
                            FilterBuffer *zbuff_axbx1,
                            FilterBuffer *zbuff_axbx2,
                            FilterBuffer *zbuff_axbx3,
                            FilterBuffer *zbuff_axbx4 );

// Циклическое перемещение указателей буфера
void Moving_Pointers( FilterBuffer *zbuff);