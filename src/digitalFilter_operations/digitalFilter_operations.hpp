#define BUFF_SIZE 2

int32_t sum_p(int32_t * sum_num1, int32_t * sum_num2);

int32_t mult_p(int32_t * mult_num1, int32_t * mult_num2);

// Объявление буферов
struct FilterBuffer {
    int32_t buffer[BUFF_SIZE];  // Буфер должен быть массивом
    int32_t *in;                // Указатель на текущую позицию ввода
    int32_t *out;               // Указатель на текущую позицию вывода
};

// Хранение промежуточных значений фильтра
struct out_values {
    int32_t b0_out;
    int32_t b1_out;
    int32_t a1_out;
    int32_t y_out;
};

// Коэффициенты фильтра
struct coefs {
    int32_t b0;
    int32_t b1;
    int32_t a1;
};