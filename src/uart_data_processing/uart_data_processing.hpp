// Отправляем строку символов через интерфейс USART в блокирующем режиме
void UART_Send_String(uint32_t usart, const char *str);

// Преобразуем целое число в строку
void UART_Send_Decimal (uint32_t usart, const int64_t num);

// Функция для чтения строки из 8+1 символов (8 символов + нуль-терминатор)
void UART_Read_String(const uint16_t MAX_STR_LEN);

// Функция для проверки готовности строки
bool Is_String_Ready(void);

// Функция для получения строки
const char* Get_Received_String(void);