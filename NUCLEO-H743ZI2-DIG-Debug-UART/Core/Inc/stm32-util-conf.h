#ifndef __STM32UTIL_CONF_H__
#define __STM32UTIL_CONF_H__

#define STM32UTIL_DEBUG_UART_USE_LL             1
#define STM32UTIL_DEBUG_UART_ON                 1
#define STM32UTIL_DEBUG_UART_TX_BUFFER_SIZE     1024

// HAL
#define STM32UTIL_DEBUG_HUART                   huart8

// LL
#define STM32UTIL_DEBUG_UART                    USART3
#define STM32UTIL_DEBUG_UART_DMA                DMA1
#define STM32UTIL_DEBUG_UART_RUNNEL             LL_DMA_RUNNEL_7 /* runnel <- channel or stream */

#if 0
// HAL
#define STM32UTIL_DEBUG_UART_STDOUT_HUART       huart8
#define STM32UTIL_DEBUG_UART_STDERR_HUART       huart7

// LL
#define STM32UTIL_DEBUG_UART_STDOUT_UART        USART3
#define STM32UTIL_DEBUG_UART_STDOUT_DMA         DMA1
#define STM32UTIL_DEBUG_UART_STDOUT_RUNNEL      LL_DMA_RUNNEL_7 /* runnel <- channel or stream */
#define STM32UTIL_DEBUG_UART_STDERR_UART        USART1
#define STM32UTIL_DEBUG_UART_STDERR_DMA         DMA2
#define STM32UTIL_DEBUG_UART_STDERR_RUNNEL      LL_DMA_RUNNEL_7

#define STM32UTIL_DEBUG_UART_USE_STDERR         0
#define STM32UTIL_DEBUG_UART_OUT_BOTH           0
#endif

#endif /* __STM32UTIL_CONF_H__ */
