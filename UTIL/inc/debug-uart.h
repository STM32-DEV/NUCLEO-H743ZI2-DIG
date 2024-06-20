#if !defined(__STM32UTIL_DEBUG_UART_H__)
#define __STM32UTIL_DEBUG_UART_H__

#include "stm32-util-conf.h"
#include "stm32-base.h"

#if !STM32UTIL_DEBUG_UART_USE_LL
#define STM32UTIL_DEBUG_UART_USE_HAL		1
#endif

#if !defined(STM32UTIL_DEBUG_UART_USE_STDERR)
#define STM32UTIL_DEBUG_UART_USE_STDERR		0
#endif

#if STM32UTIL_DEBUG_UART_USE_HAL && STM32UTIL_DEBUG_UART_USE_LL
#error "Configuration error: Both HAL and LL drivers are defined. Please define only one: STM32UTIL_DEBUG_UART_USE_HAL or STM32UTIL_DEBUG_UART_USE_LL."
#endif

#if defined(DEBUG)
#	if !defined(STM32UTIL_DEBUG_UART_ON)
#		define STM32UTIL_DEBUG_UART_ON	1
#	endif
#endif

#if defined(STM32UTIL_DEBUG_UART_USE_HAL)
#	if !defined(STM32UTIL_DEBUG_HUART)
#		error "STM32UTIL_DEBUG_HUART must be defined."
#	endif
#endif

#if defined(STM32UTIL_DEBUG_UART_USE_LL)
#	if !defined(STM32UTIL_DEBUG_UART)
#		error "STM32UTIL_DEBUG_UART must be defined."
#	endif
#endif

#if !defined(STM32UTIL_DEBUG_UART_OUT_BOTH)
#define STM32UTIL_DEBUG_UART_OUT_BOTH		0
#endif

#if !defined(STM32UTIL_DEBUG_UART_TX_BUFFER_SIZE)
#define STM32UTIL_DEBUG_UART_TX_BUFFER_SIZE 1024
#endif

#if !IS_POWER_OF_2(STM32UTIL_DEBUG_UART_TX_BUFFER_SIZE)
#error "The size of the STM32UTIL_DEBUG_UART_TX_BUFFER_SIZE must be a power of 2 and greater than zero."
#endif

#if STM32UTIL_DEBUG_UART_RUNNEL == LL_DMA_RUNNEL_0
#define DEBUG_UART_RUNNEL_NUM	0
#elif STM32UTIL_DEBUG_UART_RUNNEL == LL_DMA_RUNNEL_1
#define DEBUG_UART_RUNNEL_NUM	1
#elif STM32UTIL_DEBUG_UART_RUNNEL == LL_DMA_RUNNEL_2
#define DEBUG_UART_RUNNEL_NUM	2
#elif STM32UTIL_DEBUG_UART_RUNNEL == LL_DMA_RUNNEL_3
#define DEBUG_UART_RUNNEL_NUM	3
#elif STM32UTIL_DEBUG_UART_RUNNEL == LL_DMA_RUNNEL_4
#define DEBUG_UART_RUNNEL_NUM	4
#elif STM32UTIL_DEBUG_UART_RUNNEL == LL_DMA_RUNNEL_5
#define DEBUG_UART_RUNNEL_NUM	5
#elif STM32UTIL_DEBUG_UART_RUNNEL == LL_DMA_RUNNEL_6
#define DEBUG_UART_RUNNEL_NUM	6
#elif STM32UTIL_DEBUG_UART_RUNNEL == LL_DMA_RUNNEL_7
#define DEBUG_UART_RUNNEL_NUM	7
#endif

#define STM32UTIL_DEBUG_UART_DMA_IsActiveFlag_TC()	CONCAT_TOKEN(LL_DMA_IsActiveFlag_TC, TO_NUM(DEBUG_UART_RUNNEL_NUM))(STM32UTIL_DEBUG_UART_DMA)
#define STM32UTIL_DEBUG_UART_DMA_ClearFlag_TC()		CONCAT_TOKEN(LL_DMA_ClearFlag_TC, TO_NUM(DEBUG_UART_RUNNEL_NUM))(STM32UTIL_DEBUG_UART_DMA)
/*_AT_ITCMRAM */ extern void STM32UTIL_DEBUG_UART_DMA_TC_isr();
/*_AT_ITCMRAM */ extern void STM32UTIL_DEBUG_UART_TC_isr();

#define STM32UTIL_DEBUG_GROUP_ALL	0xFFFFFFFF
#define STM32UTIL_DEBUG_GROUP_DEFAULT	0x00000001
#define STM32UTIL_DEBUG_GROUP_ETH	0x80000000
#define STM32UTIL_DEBUG_GROUP_SDMMC	0x40000000
#define STM32UTIL_DEBUG_GROUP_FATFS	0x20000000
#define STM32UTIL_DEBUG_GROUP_HSM	0x10000000

#if !defined(STM32UTIL_DEBUG_GROUP)
#define STM32UTIL_DEBUG_GROUP		STM32UTIL_DEBUG_GROUP_DEFAULT
#endif

#if !defined(STM32UTIL_DEBUG_GROUP_CURRENT)
#define STM32UTIL_DEBUG_GROUP_CURRENT	STM32UTIL_DEBUG_GROUP_DEFAULT
#endif

#if !defined(STM32UTIL_DEBUG_LEVEL)
#define STM32UTIL_DEBUG_LEVEL		9	// -1: disable / 0 ~ 9
#endif

#define STM32UTIL_DEBUG_LEVEL_PRINTF	0
#define STM32UTIL_DEBUG_LEVEL_DPRINTF	1
#define STM32UTIL_DEBUG_LEVEL_ERROR	2
#define STM32UTIL_DEBUG_LEVEL_WARNING	3
#define STM32UTIL_DEBUG_LEVEL_TRACE	4

#define DBG_WRAP(a)		do if (dbggetgroup() & STM32UTIL_DEBUG_GROUP) { a; } while (0)

#if STM32UTIL_DEBUG_UART_ON && STM32UTIL_DEBUG_LEVEL_PRINTF <= STM32UTIL_DEBUG_LEVEL
#define PRINTF(...)		DBG_WRAP( printf(__VA_ARGS__) )
#else
#define PRINTF(...)
#endif

#if STM32UTIL_DEBUG_UART_ON && STM32UTIL_DEBUG_LEVEL_DPRINTF <= STM32UTIL_DEBUG_LEVEL
#define DPRINTF(...)		DBG_WRAP( dbgprintf(STM32UTIL_DEBUG_GROUP_CURRENT, STM32UTIL_DEBUG_LEVEL_DPRINTF, __LINE__, __func__, __FILE__, __VA_ARGS__) )
#else
#define DPRINTF(...)
#endif

#if STM32UTIL_DEBUG_UART_ON && STM32UTIL_DEBUG_LEVEL_ERROR <= STM32UTIL_DEBUG_LEVEL
#define ERROR(...)		DBG_WRAP( dbgprintf(STM32UTIL_DEBUG_GROUP_CURRENT, STM32UTIL_DEBUG_LEVEL_ERROR, __LINE__, __func__, __FILE__, __VA_ARGS__) )
#else
#define ERROR(...)
#endif

#if STM32UTIL_DEBUG_UART_ON && STM32UTIL_DEBUG_LEVEL_WARNING <= STM32UTIL_DEBUG_LEVEL
#define WARNING(...)		DBG_WRAP( dbgprintf(STM32UTIL_DEBUG_GROUP_CURRENT, STM32UTIL_DEBUG_LEVEL_WARNING, __LINE__, __func__, __FILE__, __VA_ARGS__) )
#else
#define WARNING(...)
#endif

#if STM32UTIL_DEBUG_UART_ON && STM32UTIL_DEBUG_LEVEL_TRACE <= STM32UTIL_DEBUG_LEVEL
#define TRACE(...)		DBG_WRAP( dbgprintf(STM32UTIL_DEBUG_GROUP_CURRENT, STM32UTIL_DEBUG_LEVEL_TRACE, __LINE__, __func__, __FILE__, __VA_ARGS__) )
#else
#define TRACE(...)
#endif

/**

 */
#if STM32UTIL_DEBUG_UART_ON
#define CRITICAL(msg) do { \
		static const char critical_loc_str[] = "() " __FILE__ "(" TOSTRING(__LINE__) ")"; \
		dbgcritical(msg, sizeof msg, __func__, sizeof __func__, critical_loc_str, sizeof critical_loc_str); \
	} while (0)
#else
#define CRITICAL(msg)		dbgcritical(msg, sizeof msg, NULL, 0, NULL, 0)
#endif

/**

 */
#define ASSERT(cond, ...) do { \
		if (!(cond)) { \
			dbgassert(#cond, sizeof #cond); \
		} \
	} while (0)

#if defined(__cplusplus)
extern "C" {
#endif

uint32_t dbggetgroup();
uint32_t dbgsetgroup(uint32_t group);
uint32_t dbgresetgroup(uint32_t group);
int dbggetlevel();
int dbgsetlevel(int level);
int dbgprintf(uint32_t group, int level, int line, const char* func, const char* file, const char* fmt, ...);

void dbgcritical(const char* msg, int msglen, const char* func, int funclen, const char* loc, int loclen) __attribute__((noreturn));

// bool debug_uart_init();

#if defined(__cplusplus)
}
#endif

#endif // __STM32UTIL_DEBUG_UART_H__
