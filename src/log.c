#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* ---- Types ---- */

#if !defined(PPE_CFG_LOG_BUFFER_SIZE) || (PPE_CFG_LOG_BUFFER_SIZE < 128)
#undef PPE_CFG_LOG_BUFFER_SIZE
#define PPE_CFG_LOG_BUFFER_SIZE 4096
#endif

typedef struct _PPE_LOG
{
    ppe_log_write_vfn write;
    ppe_log_flush_vfn flush;

    ppe_uint32 threshold:4;
    ppe_uint32 flags:28;
    ppe_size buf_cap;
    char buf[PPE_CFG_LOG_BUFFER_SIZE];
} ppe_log_st;

/* ---- Preset Values ---- */

#if defined(PPE_CFG_OS_WINDOWS)

static const char newline_s[] = { '\r', '\n' };

#else

static const char newline_s[] = { '\n' };

#endif

static ppe_log_st stderr_logger_s = {
    &ppe_log_write_cfn,
    &ppe_log_flush_cfn,
    PPE_LOG_WARN,
    PPE_LOG_DEFAULT_FORMAT,
    PPE_CFG_LOG_BUFFER_SIZE,
    ""
};

static ppe_log stderr_log_s = &stderr_logger_s;

/* ---- Functions ---- */

PPE_API ppe_log ppe_log_create(ppe_size buf_cap, ppe_log_level threshold, ppe_uint32 flags, ppe_log_write_vfn write, ppe_log_flush_vfn flush)
{
    ppe_log log = NULL;
    
    if (buf_cap == 0) buf_cap = PPE_CFG_LOG_BUFFER_SIZE;

    log = (ppe_log) calloc(1, sizeof(ppe_log_st) - sizeof(log->buf) + buf_cap);
    if (! log) return NULL;

    log->threshold = threshold;
    log->flags = flags;
    log->write = write;
    log->flush = flush;
    log->buf_cap = buf_cap;
    return log;
}

PPE_API void ppe_log_destroy(ppe_log restrict log)
{
    if (log && log != stderr_log_s) free(log);
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

PPE_API ppe_log_level ppe_log_set_threshold(ppe_log restrict log, ppe_log_level level)
{
    ppe_log_level last = 0;
    assert(log);
    last = log->threshold;
    log->threshold = level;
    return last;
}

PPE_API ppe_log_level ppe_log_get_threshold(ppe_log restrict log)
{
    assert(log);
    return log->threshold;
}

static ppe_bool ppe_log_write_cfn(ppe_log_itf restrict itf, const char * buf, ppe_size size)
{
    if (size == 0) return ppe_true;
    return fwrite(stderr, buf, size) == size;
}

static ppe_bool ppe_log_flush_cfn(ppe_log_itf restrict itf)
{
    return fflush(stderr) == EOF;
}

PPE_API ppe_log_itf ppe_log_get_stderr_logger(void)
{
    return &stderr_log_s;
}

static ppe_log_itf default_logger_s = &stderr_log_s;

PPE_API ppe_log_itf ppe_log_set_global_default(ppe_log_itf restrict itf)
{
    ppe_log last = default_logger_s;
    default_logger_s = itf;
    return last;
}

PPE_API ppe_log_itf ppe_log_get_global_default(void)
{
    return default_logger_s;
}

static ppe_size ppe_log_format_tags(ppe_log restrict log, ppe_log_level level, const char * restrict where)
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

    if (log->flags & PPE_LOG_MILLISECOND) {
        ret = snprintf(log->buf + size, log->buf_cap - size, "%04d-%02d-%02d %02d:%02d:%02d.%03d ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, (int)(tv.tv_usec / 1000));
    } else {
        ret = snprintf(log->buf + size, log->buf_cap - size, "%04d-%02d-%02d %02d:%02d:%02d ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    }
    if (ret < 0) {
        /* TODO: Error Handling */
        return size;
    }
    size += ret;

    /* TODO: Thread ID Tag */

    /* Where Tag */
    if (! (log->flags & PPE_LOG_LONG_FILENAME)) {
#if defined(PPE_CFG_OS_WINDOWS)
        pos = strrchr(where, '\\');
        if (! pos) = strrchr(where, '/');
#else
        pos = strrchr(where, '/');
#endif

        if (pos) where = pos;
    }

    len = strlen(where);
    if (size + len >= log->buf_cap - 1) return size;

    memcpy(log->buf + size, where, len);
    size += len;
    log->buf[size++] = ' ';

    /* Level Tag */
    if (size + level_tags_size_l[level] >= log->buf_cap - 1) return size;

    memcpy(log->buf + size, level_tags_l[level], level_tags_size_l[level]);
    size += level_tags_size_l[level];
    log->buf[size++] = ' ';

    return size;
}

PPE_API void ppe_log_write_to(ppe_log_itf restrict itf, ppe_log_level level, const char * restrict where, const char * msg, ppe_size size)
{
    ppe_log log = NULL;
    ppe_size tags_size = 0;

    assert(where);
    assert(PPE_LOG_DEBUG <= level && level <= PPE_LOG_FATAL);
    assert(msg);

    if (! itf) itf = ppe_log_get_default();
    log = *itf;

    if (level < log->threshold) return;

    tags_size = ppe_log_format_tags(log, level, where);
    if (! log->write(itf, log->buf, tags_size)) return;

    if (! log->write(itf, msg, size > 0 ? size : strlen(msg))) return;
    if (! log->write(itf, newline_s, sizeof(newline_s))) return;

    log->flush(itf);
}

PPE_API void ppe_log_vprintf_to(ppe_log_itf restrict itf, ppe_log_level level, const char * restrict where, const char * restrict fmt, va_list args)
{
    ppe_log log = NULL;
    ppe_size size = 0;
    int ret = 0;

    assert(where);
    assert(PPE_LOG_DEBUG <= level && level <= PPE_LOG_FATAL);
    assert(fmt && strchr(fmt, '%'));

    if (! itf) itf = ppe_log_get_default();
    log = *itf;

    if (level < log->threshold) return;
    
    size = ppe_log_format_tags(log, level, where);
    ret = vsnprintf(log->buf + size, log->buf_cap - size, fmt, args);
    if (ret < 0) {
        if (! log->write(itf, log->buf, size)) return;

        size = 0;
        ret = vsnprintf(log->buf, log->buf_cap, fmt, args);
        if (ret < 0) {
            /* TODO: Error Handling */
            return;
        }
    }
    size += ret;

#if defined(PPE_CFG_OS_WINDOWS)

    if (size < log->buf_cap - 1) {
        log->buf[size++] = '\r';
        log->buf[size++] = '\n';
        if (! log->write(itf, log->buf, size)) return;

        log->flush(itf);
        return;
    }

#else

    if (size < log->buf_cap) {
        log->buf[size++] = '\n';
        if (! log->write(itf, log->buf, size)) return;

        log->flush(itf);
        return;
    }

#endif

    if (! log->write(itf, log->buf, size)) return;
    if (! log->write(itf, newline_s, sizeof(newline_s))) return;
    log->flush(itf);
    return;
}

/* ---- Wrappers ---- */

#ifdef __cplusplus
}
#endif

