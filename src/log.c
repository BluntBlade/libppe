#include <assert.h>
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

/* ---- Preset Values ---- */

#if defined(PPE_CFG_OS_WINDOWS)

static const char newline_s[] = { '\r', '\n' };

#else

static const char newline_s[] = { '\n' };

#endif

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

static ppe_log_level threshold_s = PPE_LOG_INFO;

PPE_API ppe_log_level ppe_log_set_threshold(ppe_log_level level)
{
    ppe_log_level last = threshold_s;
    threshold_s = level;
    return last;
}

PPE_API ppe_log_level ppe_log_get_threshold(void)
{
    return threshold_s;
}

static ppe_log_itf default_logger_s = PPE_LOG_STDERR_LOGGER;

PPE_API ppe_log_itf ppe_log_set_default(ppe_log_itf restrict log)
{
    ppe_log last = default_logger_s;
    default_logger_s = log;
    return last;
}

PPE_API ppe_log_itf ppe_log_get_default(void)
{
    return default_logger_s;
}

PPE_API void ppe_log_printf_to(ppe_log_itf restrict log, ppe_log_level level, const char * restrict where, const char * fmt, ...)
{
    va_list args;

    assert(fmt && strchr(fmt, '%'));

    if (level < threshold_s) return;

    va_start(args, fmt);
    ppe_log_vprintf_to(default_logger_s, level, fmt, args);
    va_end(args, fmt);
}

#if !defined(PPE_CFG_LOG_BUFFER_SIZE) || (PPE_CFG_LOG_BUFFER_SIZE < 128)
#undef PPE_CFG_LOG_BUFFER_SIZE
#define PPE_CFG_LOG_BUFFER_SIZE 4096
#endif

static char buffer_s[PPE_CFG_LOG_BUFFER_SIZE];

static ppe_size ppe_log_format_tags(ppe_log_level, const char * restrict where, char * restrict buf, ppe_size cap)
{
    static const char * const level_tags_l[] = {
        "[DEBUG]",
        "[INFO]",
        "[WARN]",
        "[ERROR]",
        "[FATAL]"
    };

    ppe_size size = 0;
    int i = 0;

    /* TODO: Timestamp Tag */

    /* TODO: Thread ID Tag */

    /* Where Tag */
    i = 0;
    while((buf[size++] = where[i++]) != '\0');

    /* Level Tag */
    i = 0;
    while((buf[size++] = level_tags_l[level][i++]) != '\0');

    buf[size] = '\0';
    return size;
}

static void ppe_log_output_to(ppe_log_itf restrict log, const char * buf, ppe_size size)
{
    int ret = 0;

    if (log) {
        (*log)->out(log, buf, size);
        return;
    }

    ret = fwrite(stderr, buf, size);
    if (ret < 0) {
        /* TODO: PANIC */
        return;
    }
}

PPE_API void ppe_log_vprintf_to(ppe_log_itf restrict log, ppe_log_level level, const char * restrict where, const char * restrict fmt, va_list args)
{
    ppe_size size = ppe_log_format_tags(level, where, buffer_s, sizeof(buffer_s));
    int ret = vsnprintf(buffer_s + size, sizeof(buffer_s) - size, fmt, args);
    if (ret < 0) {
        /* TODO: PANIC */
        return;
    }
    size += ret;

#if defined(PPE_CFG_OS_WINDOWS)

    if (size < sizeof(buffer_s) - 1) {
        buffer_s[size++] = '\r';
        buffer_s[size++] = '\n';
        ppe_log_output_to(log, buffer_s, size);
        return;
    }

#else

    if (size < sizeof(buffer_s)) {
        buffer_s[size++] = '\n';
        ppe_log_output_to(log, buffer_s, size);
        return;
    }

#endif

    ppe_log_output_to(log, buffer_s, size);
    ppe_log_output_to(log, newline_s, sizeof(newline_s));
    return;
}

/* ---- Wrappers ---- */

PPE_API void ppe_log_debug(const char * restrict where, const char * msg, ppe_size size)
{
    ppe_size tags_size = 0;

    assert(msg && ! strchr(msg, '%'));

    if (PPE_LOG_DEBUG < threshold_s) return;

    tags_size = ppe_log_format_tags(PPE_LOG_DEBUG, where, buffer_s, sizeof(buffer_s));
    ppe_log_output_to(default_logger_s, buffer_s, tags_size);

    ppe_log_output_to(default_logger_s, msg, size > 0 ? size : strlen(msg));
    ppe_log_output_to(default_logger_s, newline_s, sizeof(newline_s));
}

PPE_API void ppe_log_info(const char * restrict where, const char * msg, ppe_size size)
{
    ppe_size tags_size = 0;

    assert(msg && ! strchr(msg, '%'));

    if (PPE_LOG_INFO < threshold_s) return;

    tags_size = ppe_log_format_tags(PPE_LOG_INFO, where, buffer_s, sizeof(buffer_s));
    ppe_log_output_to(default_logger_s, buffer_s, tags_size);

    ppe_log_output_to(default_logger_s, msg, size > 0 ? size : strlen(msg));
    ppe_log_output_to(default_logger_s, newline_s, sizeof(newline_s));
}

PPE_API void ppe_log_warn(const char * restrict where, const char * msg, ppe_size size)
{
    ppe_size tags_size = 0;

    assert(msg && ! strchr(msg, '%'));

    if (PPE_LOG_WARN < threshold_s) return;

    tags_size = ppe_log_format_tags(PPE_LOG_WARN, where, buffer_s, sizeof(buffer_s));
    ppe_log_output_to(default_logger_s, buffer_s, tags_size);

    ppe_log_output_to(default_logger_s, msg, size > 0 ? size : strlen(msg));
    ppe_log_output_to(default_logger_s, newline_s, sizeof(newline_s));
}

PPE_API void ppe_log_error(const char * restrict where, const char * msg, ppe_size size)
{
    ppe_size tags_size = 0;

    assert(msg && ! strchr(msg, '%'));

    if (PPE_LOG_ERROR < threshold_s) return;

    tags_size = ppe_log_format_tags(PPE_LOG_ERROR, where, buffer_s, sizeof(buffer_s));
    ppe_log_output_to(default_logger_s, buffer_s, tags_size);

    ppe_log_output_to(default_logger_s, msg, size > 0 ? size : strlen(msg));
    ppe_log_output_to(default_logger_s, newline_s, sizeof(newline_s));
}

PPE_API void ppe_log_fatal(const char * restrict where, const char * msg, ppe_size size)
{
    ppe_size tags_size = 0;

    assert(msg && ! strchr(msg, '%'));

    tags_size = ppe_log_format_tags(PPE_LOG_FATAL, where, buffer_s, sizeof(buffer_s));
    ppe_log_output_to(default_logger_s, buffer_s, tags_size);

    ppe_log_output_to(default_logger_s, msg, size > 0 ? size : strlen(msg));
    ppe_log_output_to(default_logger_s, newline_s, sizeof(newline_s));
}

PPE_API void ppe_log_debugf(const char * restrict where, const char * fmt, ...)
{
    va_list args;

    assert(fmt && strchr(fmt, '%'));

    if (PPE_LOG_DEBUG < threshold_s) return;

    va_start(args, fmt);
    ppe_log_vprintf_to(default_logger_s, PPE_LOG_DEBUG, where, fmt, args);
    va_end(args, fmt);
}

PPE_API void ppe_log_infof(const char * restrict where, const char * fmt, ...)
{
    va_list args;

    assert(fmt && strchr(fmt, '%'));

    if (PPE_LOG_INFO < threshold_s) return;

    va_start(args, fmt);
    ppe_log_vprintf_to(default_logger_s, PPE_LOG_INFO, where, fmt, args);
    va_end(args, fmt);
}

PPE_API void ppe_log_warnf(const char * restrict where, const char * fmt, ...)
{
    va_list args;

    assert(fmt && strchr(fmt, '%'));

    if (PPE_LOG_WARN < threshold_s) return;

    va_start(args, fmt);
    ppe_log_vprintf_to(default_logger_s, PPE_LOG_WARN, where, fmt, args);
    va_end(args, fmt);
}

PPE_API void ppe_log_errorf(const char * restrict where, const char * fmt, ...)
{
    va_list args;

    assert(fmt && strchr(fmt, '%'));

    if (PPE_LOG_ERROR < threshold_s) return;

    va_start(args, fmt);
    ppe_log_vprintf_to(default_logger_s, PPE_LOG_ERROR, where, fmt, args);
    va_end(args, fmt);
}

PPE_API void ppe_log_fatalf(const char * restrict where, const char * fmt, ...)
{
    va_list args;

    assert(fmt && strchr(fmt, '%'));

    va_start(args, fmt);
    ppe_log_vprintf_to(default_logger_s, PPE_LOG_FATAL, where, fmt, args);
    va_end(args, fmt);
}

#ifdef __cplusplus
}
#endif
