#define DATA_SIZE 1544  // размер принятого кадра
#define COLS 32          // количество столбцов двумерного массива температуры
#define ROWS 24          // количество строк 

// тактирование
void Clock_Setup(void);

// настройка UART для обмена данными с датчиком
void UART_GYMCU90640_Setup();

// настраиваем UART, по которому будем видеть обработанные данные с датчика
void UART_Data_Setup();

// // Отправляем строку символов через интерфейс USART в блокирующем режиме
// void UART_Send_String(uint32_t usart, char* str);

// // функция для отправки команд
// void USART_Send_Command(uint8_t *data);


// структура для хранения необработанных данных с датчика
typedef struct infrared_sensor_data {
    uint8_t frame_buffer[DATA_SIZE];        // буффер для хранения кадра данных с датчика

    uint16_t buffer_index;                  // текущий индекс буффера
    
    bool frame_ready;                       // флаг принятия кадра
};

typedef struct ready_made_data {
    uint16_t data_volume;           // 0x06 * 256 + 0x02 = 1538 -- объем данных
                                    // о температуре
                                            
    float temperature[ROWS][COLS];  // температуре в каждой точке

    float ambient_temperature;      // температура окружающей среды 
                                    // (собственная температура датчика)
    
    float cumulative_sum;           // совокупная сумма первых 771 слов (16 бит)
};

// НУЖНО СОЗДАТЬ ОТДЕЛЬНУЮ СТРУКТУРУ, КУДА БУДЕМ ЗАПИСЫВАТЬ ВЫЧИСЛЕННЫЕ ДАННЫЕ

// обработка данных с датчика
void UART_Processing(infrared_sensor_data *sensor);

// обработка  кадра
void Frame_Processing(infrared_sensor_data* sensor, ready_made_data* ready_data);

