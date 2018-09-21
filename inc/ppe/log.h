#ifndef PPE_LOG_H
#define PPE_LOG_H 1

#include <stdarg.h>

#include "ppe/types.h"
#include "ppe/macros.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ---- Types ---- */

struct PPE_LOG;
typedef struct PPE_LOG * ppe_log;
typedef struct PPE_LOG ** ppe_log_itf;

typedef enum _PPE_LOG_LEVEL
{
    PPE_LOG_DEBUG = 0,
    PPE_LOG_INFO = 1,
    PPE_LOG_WARN = 2,
    PPE_LOG_ERROR = 3,
    PPE_LOG_FATAL = 4
} ppe_log_level;

/* ---- Preset Values ---- */

#define PPE_LOG_STDERR_LOGGER NULL

/* ---- Interfaces ---- */

typdef void (*ppe_log_output_vfn)(ppe_log_itf restrict itf, const char * msg, ppe_size size);

/* ---- Functions ---- */

PPE_API extern ppe_log ppe_log_create(ppe_log_output_vfn out);
PPE_API extern void ppe_log_destroy(ppe_log restrict log);

PPE_API extern void ppe_log_printf_to(ppe_log_itf restrict log, ppe_log_level level, const char * fmt, ...);
PPE_API extern void ppe_log_vprintf_to(ppe_log_itf restrict log, ppe_log_level level, const char * fmt, va_list args);

PPE_API extern ppe_log_level ppe_log_set_threshold(ppe_log_level level);
PPE_API extern ppe_log_level ppe_log_get_threshold(void);

PPE_API extern ppe_log_itf ppe_log_set_default(ppe_log_itf restrict log);
PPE_API extern ppe_log_itf ppe_log_get_default(void);

/* ---- Wrappers ---- */

PPE_API extern void ppe_log_debug(const char * msg);
PPE_API extern void ppe_log_info(const char * msg);
PPE_API extern void ppe_log_warn(const char * msg);
PPE_API extern void ppe_log_error(const char * msg);
PPE_API extern void ppe_log_fatal(const char * msg);

PPE_API extern void ppe_log_debugf(const char * fmt, ...);
PPE_API extern void ppe_log_infof(const char * fmt, ...);
PPE_API extern void ppe_log_warnf(const char * fmt, ...);
PPE_API extern void ppe_log_errorf(const char * fmt, ...);
PPE_API extern void ppe_log_fatalf(const char * fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* PPE_LOG_H */

