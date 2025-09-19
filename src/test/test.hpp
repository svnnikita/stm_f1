// структура данных
#define MAX_STR_LEN_CONST 16   				// максимальная длина строки

// объявим структуру для хранения строк и чисел
struct data {
	uint8_t symbol; 								// принятый символ

	uint8_t read_string[MAX_STR_LEN_CONST];		// буфер, в который будет записываться строка

	uint8_t first_number[MAX_STR_LEN_CONST];	// первое число
	uint8_t second_number[MAX_STR_LEN_CONST]; 	// второе число

	uint8_t index;

    uint8_t string_ready = 0;
};

void UART_proc(data* strAndNums);