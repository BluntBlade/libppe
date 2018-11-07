#ifndef PPE_LOG_H
#define PPE_LOG_H 1

#include <stdarg.h>

#include "ppe/types.h"
#include "ppe/macros.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ==== Declaration ==== */

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

enum
{
    PPE_LOG_SHORT_FILENAME = 0x0,
    PPE_LOG_LONG_FILENAME = 0x1,
    PPE_LOG_MILLISECOND = 0x2
};

/* ---- Macros ---- */

#define PPE_LOG_FILE_LOCATION (__FILE__ ## ":" ## #__LINE__)

#define PPE_LOG_DEFAULT_FORMAT (PPE_LOG_SHORT_FILENAME | PPE_LOG_MILLISECOND)

/* ---- Interfaces ---- */

typedef ppe_bool (*ppe_log_write_vfn)(ppe_log_itf restrict itf, const char * msg, ppe_size size);
typedef ppe_bool (*ppe_log_flush_vfn)(ppe_log_itf restrict itf);

/* ---- Functions ---- */

/* -- Create & Destroy -- */
PPE_API extern ppe_log ppe_log_create(ppe_size buf_cap, ppe_log_level threshold, ppe_uint flags, ppe_log_write_vfn write, ppe_log_flush_vfn flush);
PPE_API extern void ppe_log_destroy(ppe_log restrict log);

/* -- Property -- */
PPE_API extern ppe_log_level ppe_log_set_threshold(ppe_log restrict log, ppe_log_level level);
PPE_API extern ppe_log_level ppe_log_get_threshold(ppe_log restrict log);
PPE_API extern ppe_uint ppe_log_set_flags(ppe_log restrict log, ppe_uint flags);
PPE_API extern ppe_uint ppe_log_get_flags(ppe_log restrict log);

/* -- Virtual -- */

PPE_API extern void ppe_log_write_to(ppe_log_itf restrict itf, const char * restrict where, ppe_log_level level, const char * msg, ppe_size size);
PPE_API extern void ppe_log_vprintf_to(ppe_log_itf restrict itf, const char * restrict where, ppe_log_level level, const char * fmt, va_list args);

PPE_API extern ppe_log_itf ppe_log_get_stderr_logger(void);

PPE_API extern ppe_log_itf ppe_log_set_global_default(ppe_log_itf restrict itf);
PPE_API extern ppe_log_itf ppe_log_get_global_default(void);

/* -- Wrapper -- */

static inline void ppe_log_printf_to(ppe_log_itf restrict itf, const char * restrict where, ppe_log_level level, const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ppe_log_vprintf_to(itf, where, level, fmt, args);
    va_end(args);
}

static inline void ppe_log_debug(const char * restrict where, const char * msg, ppe_size size)
{
    ppe_log_write_to(NULL, where, PPE_LOG_DEBUG, msg, size);
}

static inline void ppe_log_info(const char * restrict where, const char * msg, ppe_size size)
{
    ppe_log_write_to(NULL, where, PPE_LOG_INFO, msg, size);
}

static inline void ppe_log_warn(const char * restrict where, const char * msg, ppe_size size)
{
    ppe_log_write_to(NULL, where, PPE_LOG_WARN, msg, size);
}

static inline void ppe_log_error(const char * restrict where, const char * msg, ppe_size size)
{
    ppe_log_write_to(NULL, where, PPE_LOG_ERROR, msg, size);
}

static inline void ppe_log_fatal(const char * restrict where, const char * msg, ppe_size size)
{
    ppe_log_write_to(NULL, where, PPE_LOG_FATAL, msg, size);
}

static inline void ppe_log_debugf(const char * restrict where, const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ppe_log_vprintf_to(NULL, where, PPE_LOG_DEBUG, fmt, args);
    va_end(args);
}

static inline void ppe_log_infof(const char * restrict where, const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ppe_log_vprintf_to(NULL, where, PPE_LOG_INFO, fmt, args);
    va_end(args);
}

static inline void ppe_log_warnf(const char * restrict where, const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ppe_log_vprintf_to(NULL, where, PPE_LOG_WARN, fmt, args);
    va_end(args);
}

static inline void ppe_log_errorf(const char * restrict where, const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ppe_log_vprintf_to(NULL, where, PPE_LOG_ERROR, fmt, args);
    va_end(args);
}

static inline void ppe_log_fatalf(const char * restrict where, const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ppe_log_vprintf_to(NULL, where, PPE_LOG_FATAL, fmt, args);
    va_end(args);
}

#ifdef __cplusplus
}
#endif

#endif /* PPE_LOG_H */

