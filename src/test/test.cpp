#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/dma.h>

#include "test/test.hpp"

// получение и преобразование строки в число
void UART_proc(data* strAndNums) {
	strAndNums->symbol = usart_recv(USART2);

	// Эхо-отправка символа
    usart_send(USART2, strAndNums->symbol);
		
    // Если получен символ конца строки или буфер заполнен
    if (strAndNums->symbol == '\n' || strAndNums->symbol == '\r' || 
        strAndNums->index >= MAX_STR_LEN_CONST - 1) {
        
        // Завершаем строку нулевым символом
        strAndNums->read_string[strAndNums->index] = '\0';
        
        // Сбрасываем индекс для следующей строки
        strAndNums->index = 0;
        strAndNums->string_ready = 1;
    } 
    else {
        // Сохраняем символ в буфер
        strAndNums->read_string[strAndNums->index] = strAndNums->symbol;
        strAndNums->index++;
    }
}
