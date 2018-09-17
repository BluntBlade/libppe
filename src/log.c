#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* ---- Types ---- */

typedef struct _PPE_LOG
{
    ppe_log_output_vfn out;
} ppe_log_st;

/* ---- Functions ---- */

PPE_API ppe_log ppe_log_create(ppe_log_output_vfn out)
{
    ppe_log log = (ppe_log) calloc(1, sizeof(ppe_log_st));
    if (! log) return NULL;

    log->out = out;
    return log;
}

PPE_API void ppe_log_destroy(ppe_log restrict log)
{
    if (log) free(log);
}

PPE_API void ppe_log_printf_to(ppe_log_itf restrict log, ppe_log_level level, const char * fmt, ...)
{
    va_list args;

    if (level < s_ppe_log_threshold) return;

    va_start(args, fmt);
    ppe_log_vprintf_to(s_ppe_log_default, level, fmt, args);
    va_end(args, fmt);
}

static char ppe_log_buf[4096];
static const char * ppe_log_level_tags[] = {
    "[DEBUG]",
    "[INFO]",
    "[WARN]",
    "[ERROR]",
    "[FATAL]"
};

static void ppe_log_output_to(ppe_log_itf restrict log, const char * buf, ppe_size size)
{
    int ret = 0;

    if (log) {
        (*log)->out(log, buf, size);
        return;
    }

    ret = fwrite(stderr, buf, size);
    if (ret < 0) {
        /* PANIC */
        return;
    }
}

PPE_API void ppe_log_vprintf_to(ppe_log_itf restrict log, ppe_log_level level, const char * fmt, va_list args)
{
    ppe_size size = 0;
    int ret = 0;

    /* TODO: Timestamp Tag */

    memcpy(buf + size, ppe_log_level_tags[level], strlen(ppe_log_level_tags[level]));  
    size += strlen(ppe_log_level_tags[level]);

    /* TODO: Thread ID Tag */

    ret = vsnprintf(buf + size, sizeof(ppe_log_buf) - size, fmt, args);
    if (ret < 0) {
        /* PANIC */
        return;
    }
    size += ret;

    /* TODO: CRLF */
    if (size < sizeof(ppe_log_buf)) {
        ppe_log_buf[size++] = '\n';
    }

    ppe_log_output_to(log, buf, size);
}

static ppe_log_level s_ppe_log_threshold = PPE_LOG_INFO;

PPE_API ppe_log_level ppe_log_set_threshold(ppe_log_level level)
{
    ppe_log_level last = s_ppe_log_threshold;
    s_ppe_log_threshold = level;
    return last;
}

PPE_API ppe_log_level ppe_log_get_threshold(void)
{
    return s_ppe_log_threshold;
}

static ppe_log_itf s_ppe_log_default = PPE_LOG_STDERR_LOGGER;

PPE_API ppe_log_itf ppe_log_set_default(ppe_log_itf restrict log)
{
    ppe_log last = s_ppe_log_default;
    s_ppe_log_default = log;
    return last;
}

PPE_API ppe_log_itf ppe_log_get_default(void)
{
    return s_ppe_log_default;
}

/* ---- Wrappers ---- */

PPE_API void ppe_log_debug(const char * fmt, ...)
{
    va_list args;

    if (PPE_LOG_DEBUG < s_ppe_log_threshold) return;

    va_start(args, fmt);
    ppe_log_vprintf_to(s_ppe_log_default, PPE_LOG_DEBUG, fmt, args);
    va_end(args, fmt);
}

PPE_API void ppe_log_info(const char * fmt, ...)
{
    va_list args;

    if (PPE_LOG_INFO < s_ppe_log_threshold) return;

    va_start(args, fmt);
    ppe_log_vprintf_to(s_ppe_log_default, PPE_LOG_INFO, fmt, args);
    va_end(args, fmt);
}

PPE_API void ppe_log_warn(const char * fmt, ...)
{
    va_list args;

    if (PPE_LOG_WARN < s_ppe_log_threshold) return;

    va_start(args, fmt);
    ppe_log_vprintf_to(s_ppe_log_default, PPE_LOG_WARN, fmt, args);
    va_end(args, fmt);
}

PPE_API void ppe_log_error(const char * fmt, ...)
{
    va_list args;

    if (PPE_LOG_ERROR < s_ppe_log_threshold) return;

    va_start(args, fmt);
    ppe_log_vprintf_to(s_ppe_log_default, PPE_LOG_ERROR, fmt, args);
    va_end(args, fmt);
}

PPE_API void ppe_log_fatal(const char * fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    ppe_log_vprintf_to(s_ppe_log_default, PPE_LOG_FATAL, fmt, args);
    va_end(args, fmt);
}

#ifdef __cplusplus
}
#endif
