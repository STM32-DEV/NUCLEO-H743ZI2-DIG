#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stm32-base.h>
#include <unistd.h>
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "main.h"
#include "debug-uart.h"
#include "stm32_lock.h"

#if STM32UTIL_DEBUG_UART_USE_HAL
extern UART_HandleTypeDef STM32UTIL_DEBUG_UART;
#define DEBUG_UART	(&STM32UTIL_DEBUG_UART)
#else
#define DEBUG_UART	STM32UTIL_DEBUG_UART
#endif

/*----------------------------------------------------------------------------
  Debug UART Module
----------------------------------------------------------------------------*/

/* Global variables for debug group and level */
static uint32_t dbggroup = STM32UTIL_DEBUG_GROUP_ALL;
static int dbglevel = STM32UTIL_DEBUG_LEVEL;

/**
 * @brief Get the current debug group.
 *
 * @return Current debug group value.
 */
uint32_t dbggetgroup()
{
	return dbggroup;
}

/**
 * @brief Set the debug group.
 *
 * @param group New debug group to set.
 * @return Previous debug group value.
 */
uint32_t dbgsetgroup(uint32_t group)
{
	uint32_t prev = dbggroup;
	dbggroup |= group;
	return prev;
}

/**
 * @brief Reset the debug group.
 *
 * @param group Debug group to reset.
 * @return Previous debug group value.
 */
uint32_t dbgresetgroup(uint32_t group)
{
	uint32_t prev = dbggetgroup();
	dbggroup &= ~group;
	return prev;
}

/**
 * @brief Get the current debug level.
 *
 * @return Current debug level value.
 */
int dbggetlevel()
{
	return dbglevel;
}

/**
 * @brief Set the debug level.
 *
 * @param level New debug level to set.
 * @return Previous debug level value.
 */
int dbgsetlevel(int level)
{
	int prev = dbggetlevel();
	dbglevel = level;
	return prev;
}

/*----------------------------------------------------------------------------
  Helper Functions
----------------------------------------------------------------------------*/

#define DBGPRINTF_BUF_SIZE (1024)

/**
 * @brief Convert milliseconds to hours, minutes, seconds, and milliseconds.
 *
 * @param ms      Milliseconds to convert.
 * @param hours   Pointer to store the hours.
 * @param minutes Pointer to store the minutes.
 * @param seconds Pointer to store the seconds.
 * @param milis   Pointer to store the milliseconds.
 */
static void ms_to_time(uint32_t ms, int *hours, int *minutes, int *seconds, int *milis)
{
	*milis   = ms % 1000;
	*seconds = (ms / 1000) % 60;
	*minutes = (ms / (1000 * 60)) % 60;
	*hours   = (ms / (1000 * 60 * 60)) % 24;
}

/**
 * @brief Convert debug level to a character representation.
 *
 * @param level Debug level to convert.
 * @return Character representation of the debug level.
 */
static char level_to_ch(int level)
{
	char lvlch;

	switch (level) {
	case STM32UTIL_DEBUG_LEVEL_DPRINTF:	lvlch = 'D'; break;
	case STM32UTIL_DEBUG_LEVEL_ERROR:	lvlch = 'E'; break;
	case STM32UTIL_DEBUG_LEVEL_WARNING:	lvlch = 'W'; break;
	case STM32UTIL_DEBUG_LEVEL_TRACE:	lvlch = 'T'; break;
	default:				if (0 <= level && level <= 9) { lvlch = '0' + level; } else { lvlch = '?'; }
	}

	return lvlch;
}

// Safely format a string into a buffer, ensuring null-termination.
#define _safe_(n, f, b, s, ...) int n = f(b,s-1,__VA_ARGS__); if (s-1 < n) { n=s-1; (b)[n]=0; }

/**
 * @brief Create a timestamp string with debug information.
 *
 * @param buffer Buffer to store the timestamp string.
 * @param size   Size of the buffer.
 * @param level  Debug level.
 * @param line   Line number.
 * @param func   Function name.
 * @param file   File name.
 * @return Length of the created timestamp string.
 */
static int make_timestamp_info(char* buffer, size_t size, int level, int line, const char *func, const char *file)
{
	char lvlch = level_to_ch(level);

	int hours, minutes, seconds, milis;
	ms_to_time(osKernelGetTickCount(), &hours, &minutes, &seconds, &milis);

	_safe_(n1, snprintf, buffer, size, "%02d:%02d:%02d.%03d %c %s %s(%d): ", hours, minutes, seconds, milis, lvlch, func, file, line);

	return n1;
}

/**
 * @brief Print a formatted debug message with timestamp and debug information.
 *
 * @param level Debug level.
 * @param line  Line number.
 * @param func  Function name.
 * @param file  File name.
 * @param fmt   Format string.
 * @param ...   Variable arguments for the format string.
 * @return Number of characters printed.
 */
int dbgprintf(uint32_t group, int level, int line, const char* func, const char* file, const char* fmt, ...)
{
	if (dbggetlevel() < level || !(dbggroup & group)) { // runtime check
		return 0;
	}

	char* buffer = pvPortMalloc(DBGPRINTF_BUF_SIZE);
	if (buffer) {
		int n1 = make_timestamp_info(buffer, DBGPRINTF_BUF_SIZE, level, line, func, file);

		va_list vargs;
		va_start(vargs, fmt);
		_safe_(n2, vsnprintf, buffer + n1, DBGPRINTF_BUF_SIZE - n1, fmt, vargs);
		va_end(vargs);

		printf(buffer);

		vPortFree(buffer);

		return n1 + n2;
	}

	return -1;
}

static void dbgcritical_puts(const char* msg, int len);

void dbgcritical(const char* msg, int msglen, const char* func, int funclen, const char* loc, int loclen)
{
	static const char critical_error_msg[] = "\r\nCRITICAL ERROR\r\n";
	static const char critical_error_crlf[] = "\r\n\r\n\r\n\r\n";

	__disable_irq();
	while (true) {
		dbgcritical_puts(critical_error_msg, sizeof critical_error_msg);

		dbgcritical_puts(msg, msglen);
		dbgcritical_puts(critical_error_crlf, 2);

		dbgcritical_puts(func, funclen);
		dbgcritical_puts(loc, loclen);
		dbgcritical_puts(critical_error_crlf, sizeof critical_error_crlf);
	}
}

/*----------------------------------------------------------------------------
  Debug UART Initialization and Transmission
----------------------------------------------------------------------------*/

static osSemaphoreId_t debug_uart_sema;
static const osSemaphoreAttr_t debug_uart_sema_attr = {
    .name = "debug_uart_sema",
};

/**
 * @brief Callback function for UART transmission complete.
 *
 * @param hdma UART handle.
 */
#if STM32UTIL_DEBUG_UART_USE_HAL
/*_AT_ITCMRAM*/ void debug_uart_tx_completed(UART_HandleTypeDef *hdma)
{
	osSemaphoreRelease(debug_uart_sema);
}

#elif STM32UTIL_DEBUG_UART_USE_LL
/*_AT_ITCMRAM*/ void STM32UTIL_DEBUG_UART_DMA_TC_isr()
{
	//osSemaphoreRelease(debug_uart_sema);
}

/*_AT_ITCMRAM*/ void STM32UTIL_DEBUG_UART_TC_isr()
{
	osSemaphoreRelease(debug_uart_sema);
}

static void ll_setup_uart_tx_dma(USART_TypeDef* USARTx, DMA_TypeDef* DMAx, uint32_t runnel)
{
	LL_USART_EnableDMAReq_TX(USARTx);
	LL_USART_EnableIT_TC(USARTx);
	LL_DMA_EnableIT_TC(DMAx, runnel);
	LL_DMA_SetPeriphAddress(DMAx, runnel, LL_USART_DMA_GetRegAddr(USARTx, LL_USART_DMA_REG_DATA_TRANSMIT));
}

#endif

// Indicates if the debug UART is initialized
static bool debug_uart_inited = false;

// Transmit buffer for UART
/* _AT_DMATX_SRAM1 */ __attribute__((aligned(32))) static uint8_t tx_buffer[STM32UTIL_DEBUG_UART_TX_BUFFER_SIZE];

/**
 * @brief Initialize the debug UART.
 *
 * @return true if initialization is successful.
 */
bool debug_uart_init()
{
	if (!debug_uart_inited) { // first check
		osKernelLock();

		LockingData_t test;
		if (DIMOF(test.basepri) < 4) {
			CRITICAL("#define STM32_LOCK_MAX_NESTED_LEVELS 4 more in stm32_lock.h");
		}

		if (!debug_uart_inited) { // double check
			debug_uart_sema = osSemaphoreNew(1, 1, &debug_uart_sema_attr);
			if (!debug_uart_sema) {
				CRITICAL("osSemaphoreNew debug_uart");
			}
#if STM32UTIL_DEBUG_UART_USE_HAL
			if (HAL_OK != HAL_UART_RegisterCallback(DEBUG_UART, HAL_UART_TX_COMPLETE_CB_ID, debug_uart_tx_completed)) {
				CRITICAL("HAL_UART_RegisterCallback debug_uart");
			}
#elif STM32UTIL_DEBUG_UART_USE_LL
			ll_setup_uart_tx_dma(DEBUG_UART, STM32UTIL_DEBUG_UART_DMA, STM32UTIL_DEBUG_UART_RUNNEL);
#endif

			debug_uart_inited = true;
		}

		osKernelUnlock();
	}

	return true;
}

#if STM32UTIL_DEBUG_UART_USE_HAL
static void uart_transmit(UART_HandleTypeDef* huart, uint8_t* ptr, int len)
{
	HAL_UART_Transmit(huart, ptr, len, HAL_MAX_DELAY);

}

static void uart_transmit_dma(UART_HandleTypeDef* huart, uint8_t* ptr, int len)
{
	HAL_UART_Transmit_DMA(huart, ptr, len);
}

#elif STM32UTIL_DEBUG_UART_USE_LL
static void uart_transmit(USART_TypeDef* USARTx, uint8_t* ptr, int len)
{
	while (len--) {
		while (!LL_USART_IsActiveFlag_TXE_TXFNF(USARTx)) {
		}
		LL_USART_TransmitData8(USARTx, *ptr++);
	}
}

static void uart_transmit_dma(USART_TypeDef* USARTx, uint8_t* ptr, int len, DMA_TypeDef* dma, uint32_t runnel)
{
	LL_DMA_DisableRunnel(dma, runnel);
	LL_DMA_SetMemoryAddress(dma, runnel, (uint32_t)ptr);
	LL_DMA_SetDataLength(dma, runnel, len);
	LL_DMA_EnableRunnel(dma, runnel);
}
#endif

static void dbgcritical_puts(const char* msg, int len)
{
	uart_transmit(DEBUG_UART, (uint8_t*)msg, len);
}

/**
 * @brief Write function to transmit data over debug UART.
 *
 * This function implements the low-level write operation for the debug UART.
 * If the debug UART is not initialized, it transmits the data using polling mode.
 * Otherwise, it uses DMA for transmission, handling larger data by breaking it
 * into chunks.
 *
 * @param file Unused file descriptor.
 * @param ptr  Pointer to the data buffer.
 * @param len  Length of the data buffer.
 * @return Number of bytes written.
 */
int _write(int file, char* ptr, int len)
{
	// If the RTOS kernel is not running, transmit data using polling mode
	if (osKernelRunning != osKernelGetState()) {
		uart_transmit(DEBUG_UART, (uint8_t*) ptr, len);
		return len;
	}

	debug_uart_init();

	if (len < 16) {
		uart_transmit(DEBUG_UART, (uint8_t*) ptr, len);

	        // Wait for the transmission to complete, RM0433r8 p2029 Figure 573
		while (osSemaphoreAcquire(debug_uart_sema, osWaitForever) != osOK) {
		}

		return len;
	}

	/*
		Transmit the smallest chunk first, which is the remainder of the length
        	divided by the size of the transmit buffer. This makes waiting for DMA
        	transmission more efficient.
			ex) len = 555, tx max = 256
			    1st: 555 mod 256 = 43, 2nd: 256, 3rd: 256
			ex) len = 512, tx max = 256
			    1st: 512 mod 256 = 0(256), 2nd: 256
	 */
	// Initialize the remaining length to the total length to be transmitted
	int remaining_len = len;
	// Calculate the initial chunk size, which is the remainder of the buffer size
	// STM32UTIL_DEBUG_UART_TX_BUFFER_SIZE MUST be a power of 2
	int chunk = remaining_len & (STM32UTIL_DEBUG_UART_TX_BUFFER_SIZE - 1);
	if (0 == chunk) {
		chunk = STM32UTIL_DEBUG_UART_TX_BUFFER_SIZE;
	}

	// Transmit data using DMA
	while (remaining_len) {
		// Copy data to the transmit buffer
		memcpy(tx_buffer, ptr, chunk);

		// Clean the data cache to ensure data coherence
		SCB_CleanDCache_by_Addr((uint32_t*)tx_buffer, chunk);

		uart_transmit_dma(DEBUG_UART, tx_buffer, chunk
#if STM32UTIL_DEBUG_UART_USE_LL
			, STM32UTIL_DEBUG_UART_DMA, STM32UTIL_DEBUG_UART_RUNNEL
#endif
		);

	        // Wait for the transmission to complete, RM0433r8 p2060 Figure 590
		while (osSemaphoreAcquire(debug_uart_sema, osWaitForever) != osOK) {
		}

		// Move the pointer forward and adjust the remaining length
		ptr += chunk;
		remaining_len -= chunk;
		chunk = STM32UTIL_DEBUG_UART_TX_BUFFER_SIZE;
	}

	return len;
}
