/*!
  \file logger.h
  \brief Support for logging to various files, console and syslog
	Configuration in file logger.conf
*/

#ifndef _INC_CC_LOGGER_H
#define _INC_CC_LOGGER_H

#include "common/paths.h"
//#include "common/compat.h"

#include <stdarg.h>

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#define EVENTLOG "event_log"
#define	QUEUELOG	"queue_log"

#define DEBUG_M(a) { \
	a; \
}

#define VERBOSE_PREFIX_1 " "
#define VERBOSE_PREFIX_2 "  == "
#define VERBOSE_PREFIX_3 "    -- "
#define VERBOSE_PREFIX_4 "       > "

#define RESULT_SUCCESS		0
#define RESULT_SHOWUSAGE	1
#define RESULT_FAILURE		2

typedef enum __EN_TEST_FP__
{
	FP_TO_FILE = 0,
	FP_TO_SCREEN,
	FP_TO_PARAM_ERROR,
	FP_TO_BUF,
}EN_TEST_FP;

typedef enum __DBG_ENUM__
{	DBG_START = 0,
	DBG_NAME,
	DBG_PARA,
	DBG_BUF,
}DBG_ENUM;

typedef enum __SOCKET_ALL__
{
	SOCKSET_OPEN_ALL = 0x00,
	SOCKSET_CLOSE_ALL,
	SOCKSET_PARAM_ERROR,
}SOCKSET_ALL;

typedef enum __AC491_TONE__
{
	CODEC_DIAL = 0x00,
	CODEC_SILENT,	
	CODEC_BUSY ,
	CODEC_CNF ,
	CODEC_RING ,
	CODEC_HANG ,
	CODEC_STOP,
	CODEC_BUFF,
}AC491_TONE;

typedef enum __AC491_HANG__
{
	HANG_OFF = 0x00,
	HANG_ON ,
	HANG_CLOSE,
}AC491_HANG;

typedef enum __SOCKET_MONITOR__
{
	MONITOR_I_50_SEM_BIT = 0x00,
	MONITOR_I_50_DATA_BIT,
	MONITOR_I_SEAT_SEM_BIT,
	MONITOR_I_SEAT_VOC_BIT,
	MONITOR_I_SIP_SEM_BIT,
	MONITOR_I_ETH_BIT,
	MONITOR_I_ETHD_BIT,
	MONITOR_I_ADPT_SEM_BIT,
	MONITOR_I_ADPT_VOC_BIT,
	MONITOR_I_GUN_YX_BIT,
	MONITOR_I_GUN_WX_BIT,
	MONITOR_I_GUN_UART_BIT,
	MONITOR_I_FSK_BIT,
	
	MONITOR_50_I_SEM_BIT,
	MONITOR_50_I_DATA_BIT,
	MONITOR_SEAT_I_SEM_BIT,
	MONITOR_SEAT_I_VOC_BIT,
	MONITOR_SIP_I_SEM_BIT,
	MONITOR_ETH_I_BIT,
	MONITOR_ETHD_I_BIT,
	MONITOR_ADPT_I_SEM_BIT,
	MONITOR_ADPT_I_VOC_BIT,
	MONITOR_GUN_I_YX_BIT,
	MONITOR_GUN_I_WX_BIT,
	MONITOR_GUN_I_UART_BIT,
	MONITOR_FSK_I_BIT,
}SOCKET_MONITOR;


/*! Used for sending a log message */
/*!
	\brief This is the standard logger function.  Probably the only way you will invoke it would be something like this:
	inc_log(LOG_WHATEVER, "Problem with the %s Captain.  We should get some more.  Will %d be enough?\n", "flux capacitor", 10);
	where WHATEVER is one of ERROR, DEBUG, EVENT, NOTICE, or WARNING depending
	on which log you wish to output to. These are implemented as macros, that
	will provide the function with the needed arguments.

 	\param level	Type of log event
	\param file	Will be provided by the LOG_* macro
	\param line	Will be provided by the LOG_* macro
	\param function	Will be provided by the LOG_* macro
	\param fmt	This is what is important.  The format is the same as your favorite breed of printf.  You know how that works, right? :-)
 */
void inc_log(int level, const char *file, int line, const char *function, const char *fmt, ...)
	__attribute__((format(printf, 5, 6)));

void inc_backtrace(void);

/*! \brief Reload logger without rotating log files */
extern int logger_reload(void);
extern int reload_logger(int rotate);
extern int init_logger(void);
extern void close_logger(void);
extern struct logchannel *make_logchannel(char *channel, char *components, int lineno);

extern int DbgPara_Process(unsigned char *buf, int len);
extern int DbgPara_Save(void);
extern int Dbg_Socket_SendToPc(int id, unsigned char *buf, int len);


void inc_queue_log(const char *queuename, const char *callid, const char *agent, const char *event, const char *fmt, ...)
	__attribute__((format(printf, 5, 6)));

/*! Send a verbose message (based on verbose level)
 	\brief This works like inc_log, but prints verbose messages to the console depending on verbosity level set.
 	inc_verbose(VERBOSE_PREFIX_3 "Whatever %s is happening\n", "nothing");
 	This will print the message to the console if the verbose level is set to a level >= 3
 	Note the abscence of a comma after the VERBOSE_PREFIX_3.  This is important.
 	VERBOSE_PREFIX_1 through VERBOSE_PREFIX_3 are defined.
 */
void inc_verbose(const char *fmt, ...)
	__attribute__((format(printf, 1, 2)));

int inc_register_verbose(void (*verboser)(const char *string));
int inc_unregister_verbose(void (*verboser)(const char *string));

#define _A_ __FILE__, __LINE__, __PRETTY_FUNCTION__

#ifdef LOG_DEBUG
#undef LOG_DEBUG
#endif
#define __LOG_DEBUG    0
#define LOG_DEBUG      __LOG_DEBUG, _A_

#ifdef LOG_EVENT
#undef LOG_EVENT
#endif
#define __LOG_EVENT    1
#define LOG_EVENT      __LOG_EVENT, _A_

#ifdef LOG_NOTICE
#undef LOG_NOTICE
#endif
#define __LOG_NOTICE   2
#define LOG_NOTICE     __LOG_NOTICE, _A_

#ifdef LOG_WARNING
#undef LOG_WARNING
#endif
#define __LOG_WARNING  3
#define LOG_WARNING    __LOG_WARNING, _A_

#ifdef LOG_ERROR
#undef LOG_ERROR
#endif
#define __LOG_ERROR    4
#define LOG_ERROR      __LOG_ERROR, _A_

#ifdef LOG_VERBOSE
#undef LOG_VERBOSE
#endif
#define __LOG_VERBOSE  5
#define LOG_VERBOSE    __LOG_VERBOSE, _A_

#ifdef LOG_DTMF
#undef LOG_DTMF
#endif
#define __LOG_DTMF  6
#define LOG_DTMF    __LOG_DTMF, _A_

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /* _INC_CC_LOGGER_H */
