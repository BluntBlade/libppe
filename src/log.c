#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* ---- Types ---- */

typedef struct _PPE_LOG
{
    ppe_log_output_vfn out;
    ppe_log_flush_vfn flush;

    ppe_uint32 flags;
} ppe_log_st;

/* ---- Preset Values ---- */

#if defined(PPE_CFG_OS_WINDOWS)

static const char newline_s[] = { '\r', '\n' };

#else

static const char newline_s[] = { '\n' };

#endif

/* ---- Functions ---- */

PPE_API ppe_log ppe_log_create(ppe_uint32 flags, ppe_log_output_vfn out, ppe_log_flush_vfn flush)
{
    ppe_log log = (ppe_log) calloc(1, sizeof(ppe_log_st));
    if (! log) return NULL;

    log->flags = flags;
    log->out = out;
    log->flush = flush;
    return log;
}

PPE_API void ppe_log_destroy(ppe_log restrict log)
{
    if (log) free(log);
}

PPE_API void ppe_log_set_flags(ppe_log restrict log, ppe_uint32 flags)
{
    ppe_uint32 last = 0;

    assert(log);

    last = log->flags;
    log->flags = flags;
    return last;
}

PPE_API ppe_uint32 ppe_log_get_flags(ppe_log restrict log)
{
    assert(log);
    return log->flags;
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
static ppe_uint32 default_flags_s = PPE_LOG_SHORT_FILENAME;

static ppe_size ppe_log_format_tags(ppe_log_itf restrict log, ppe_log_level level, const char * restrict where, char * restrict buf, ppe_size cap)
{
    static const char * const level_tags_l[] = {
        "[DEBUG]",
        "[INFO]",
        "[WARN]",
        "[ERROR]",
        "[FATAL]"
    };
    static const ppe_size level_tags_size_l[] = {
        strlen(level_tags_l[PPE_LOG_DEBUG]),
        strlen(level_tags_l[PPE_LOG_INFO]),
        strlen(level_tags_l[PPE_LOG_WARN]),
        strlen(level_tags_l[PPE_LOG_ERROR]),
        strlen(level_tags_l[PPE_LOG_FATAL])
    };

    ppe_uint32 flags = log ? (*log)->flags : default_flags_s;
    ppe_size size = 0;
    ppe_size len = 0;
    char * pos = NULL;
    struct timeval tv;
    struct tm tm;
    int ret = 0;

    /* Timestamp Tag */
    ret = gettimeofday(&tv, NULL);
    if (ret < 0) {
        /* TODO: Error Handling */
        return size;
    }
    if (! gmtime_r(&tv.tv_sec, &tm)) {
        /* TODO: Error Handling */
        return size;
    }

    if (flags & PPE_LOG_MILLISECOND) {
        ret = snprintf(buf + size, cap - size, "%04d-%02d-%02d %02d:%02d:%02d.%03d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, (int)(tv.tv_usec / 1000));
    } else {
        ret = snprintf(buf + size, cap - size, "%04d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    }
    if (ret < 0) {
        /* TODO: Error Handling */
        return size;
    }
    size += ret;

    /* TODO: Thread ID Tag */

    /* Where Tag */
    if (! (flags & PPE_LOG_LONG_FILENAME)) {
#if defined(PPE_CFG_OS_WINDOWS)
        pos = strrchr(where, '\\');
        if (! pos) = strrchr(where, '/');
#else
        pos = strrchr(where, '/');
#endif

        if (pos) where = pos;
    }

    len = strlen(where);
    if (size + len >= cap - 1) return size;

    memcpy(buf + size, where, len);
    size += len;
    buf[size++] = ' ';

    /* Level Tag */
    if (size + level_tags_size_l[level] >= cap - 1) return size;

    memcpy(buf + size, level_tags_l[level], level_tags_size_l[level]);
    size += level_tags_size_l[level];
    buf[size++] = ' ';

    return size;
}

static ppe_bool ppe_log_output_to(ppe_log_itf restrict log, const char * buf, ppe_size size)
{
    if (size == 0) return ppe_true;
    if (log) return (*log)->out(log, buf, size);
    return fwrite(stderr, buf, size) == size;
}

static ppe_bool ppe_log_flush_to(ppe_log_itf restrict log)
{
    if (log) return (*log)->flush(log);
    return fflush(stderr) == EOF ? ppe_false : ppe_true;
}

PPE_API void ppe_log_vprintf_to(ppe_log_itf restrict log, ppe_log_level level, const char * restrict where, const char * restrict fmt, va_list args)
{
    ppe_size size = ppe_log_format_tags(log, level, where, buffer_s, sizeof(buffer_s));
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
        if (! ppe_log_output_to(log, buffer_s, size)) return;

        ppe_log_flush_to(log);
        return;
    }

#else

    if (size < sizeof(buffer_s)) {
        buffer_s[size++] = '\n';
        if (! ppe_log_output_to(log, buffer_s, size)) return;

        ppe_log_flush_to(log);
        return;
    }

#endif

    if (! ppe_log_output_to(log, buffer_s, size)) return;
    if (! ppe_log_output_to(log, newline_s, sizeof(newline_s))) return;
    ppe_log_flush_to(log);
    return;
}

/* ---- Wrappers ---- */

PPE_API void ppe_log_debug(const char * restrict where, const char * msg, ppe_size size)
{
    ppe_size tags_size = 0;

    if (PPE_LOG_DEBUG < threshold_s) return;

    tags_size = ppe_log_format_tags(default_logger_s, PPE_LOG_DEBUG, where, buffer_s, sizeof(buffer_s));
    if (! ppe_log_output_to(default_logger_s, buffer_s, tags_size)) return;

    if (! ppe_log_output_to(default_logger_s, msg, size > 0 ? size : strlen(msg))) return;
    if (! ppe_log_output_to(default_logger_s, newline_s, sizeof(newline_s))) return;

    ppe_log_flush_to(log);
}

PPE_API void ppe_log_info(const char * restrict where, const char * msg, ppe_size size)
{
    ppe_size tags_size = 0;

    if (PPE_LOG_INFO < threshold_s) return;

    tags_size = ppe_log_format_tags(default_logger_s, PPE_LOG_INFO, where, buffer_s, sizeof(buffer_s));
    if (! ppe_log_output_to(default_logger_s, buffer_s, tags_size)) return;

    if (! ppe_log_output_to(default_logger_s, msg, size > 0 ? size : strlen(msg))) return;
    if (! ppe_log_output_to(default_logger_s, newline_s, sizeof(newline_s))) return;

    ppe_log_flush_to(log);
}

PPE_API void ppe_log_warn(const char * restrict where, const char * msg, ppe_size size)
{
    ppe_size tags_size = 0;

    if (PPE_LOG_WARN < threshold_s) return;

    tags_size = ppe_log_format_tags(default_logger_s, PPE_LOG_WARN, where, buffer_s, sizeof(buffer_s));
    if (! ppe_log_output_to(default_logger_s, buffer_s, tags_size)) return;

    if (! ppe_log_output_to(default_logger_s, msg, size > 0 ? size : strlen(msg))) return;
    if (! ppe_log_output_to(default_logger_s, newline_s, sizeof(newline_s))) return;

    ppe_log_flush_to(log);
}

PPE_API void ppe_log_error(const char * restrict where, const char * msg, ppe_size size)
{
    ppe_size tags_size = 0;

    if (PPE_LOG_ERROR < threshold_s) return;

    tags_size = ppe_log_format_tags(default_logger_s, PPE_LOG_ERROR, where, buffer_s, sizeof(buffer_s));
    if (! ppe_log_output_to(default_logger_s, buffer_s, tags_size)) return;

    if (! ppe_log_output_to(default_logger_s, msg, size > 0 ? size : strlen(msg))) return;
    if (! ppe_log_output_to(default_logger_s, newline_s, sizeof(newline_s))) return;

    ppe_log_flush_to(log);
}

PPE_API void ppe_log_fatal(const char * restrict where, const char * msg, ppe_size size)
{
    ppe_size tags_size = 0;

    tags_size = ppe_log_format_tags(default_logger_s, PPE_LOG_FATAL, where, buffer_s, sizeof(buffer_s));
    if (! ppe_log_output_to(default_logger_s, buffer_s, tags_size)) return;

    if (! ppe_log_output_to(default_logger_s, msg, size > 0 ? size : strlen(msg))) return;
    if (! ppe_log_output_to(default_logger_s, newline_s, sizeof(newline_s))) return;

    ppe_log_flush_to(log);
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
