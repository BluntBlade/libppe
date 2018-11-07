#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "ppe/log.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ==== Declaration ==== */

/* ---- Types ---- */

#if !defined(PPE_CFG_LOG_BUFFER_SIZE) || (PPE_CFG_LOG_BUFFER_SIZE < 128)
#undef PPE_CFG_LOG_BUFFER_SIZE
#define PPE_CFG_LOG_BUFFER_SIZE 4096
#endif

typedef struct PPE_LOG
{
    ppe_log_write_vfn write;
    ppe_log_flush_vfn flush;

    ppe_uint threshold:4;
    ppe_uint flags:28;
    ppe_size buf_cap;
    char buf[PPE_CFG_LOG_BUFFER_SIZE];
} ppe_log_st;

/* ---- Macros ---- */

#if defined(PPE_CFG_OS_WINDOWS)
#define PPE_LOG_NEWLINE "\r\n"
#else
#define PPE_LOG_NEWLINE "\n"
#endif

/* ---- Functions ---- */

static ppe_bool log_stderr_write(ppe_log_itf restrict itf, const char * buf, ppe_size size);
static ppe_bool log_stderr_flush(ppe_log_itf restrict itf);

static ppe_size log_format_tags(ppe_log_itf restrict itf, ppe_log restrict log, ppe_log_level level, const char * restrict where);

/* ---- Preset Values ---- */

static const char * log_newline_s = PPE_LOG_NEWLINE;
static const ppe_size log_newline_size_s = sizeof(log_newline_s) - 1;

static ppe_log_st log_stderr_s = {
    &log_stderr_write,
    &log_stderr_flush,
    PPE_LOG_WARN,
    PPE_LOG_DEFAULT_FORMAT,
    PPE_CFG_LOG_BUFFER_SIZE,
    ""
};

static ppe_log log_stderr_logger_s = &log_stderr_s;

static ppe_log_itf log_default_logger_s = &log_stderr_logger_s;

/* ==== Definitions ==== */

/* -- Internals -- */

static ppe_bool log_stderr_write(ppe_log_itf restrict itf, const char * buf, ppe_size size)
{
    if (size == 0) return ppe_true;
    return fwrite(buf, 1, size, stderr) == size;
}

static ppe_bool log_stderr_flush(ppe_log_itf restrict itf)
{
    return fflush(stderr) == EOF;
}

static ppe_size log_format_tags(ppe_log_itf restrict itf, ppe_log restrict log, ppe_log_level level, const char * restrict where)
{
#define LOG_DEBUG_TAG "[DEBUG]"
#define LOG_INFO_TAG "[INFO]"
#define LOG_WARN_TAG "[WARN]"
#define LOG_ERROR_TAG "[ERROR]"
#define LOG_FATAL_TAG "[FATAL]"
    static const char * const level_tags_l[] = {
        LOG_DEBUG_TAG,
        LOG_INFO_TAG,
        LOG_WARN_TAG,
        LOG_ERROR_TAG,
        LOG_FATAL_TAG
    };
    static const ppe_size level_tags_size_l[] = {
        sizeof(LOG_DEBUG_TAG) - 1,
        sizeof(LOG_INFO_TAG) - 1,
        sizeof(LOG_WARN_TAG) - 1,
        sizeof(LOG_ERROR_TAG) - 1,
        sizeof(LOG_FATAL_TAG) - 1
    };

#define LOG_ERR_GETTIMEOFDAY 0
#define LOG_ERR_GMTIME_R 1
#define LOG_ERR_SNPRINTF 2
#define LOG_ERR_NOSPACE_TIMESTAMP 3
#define LOG_ERR_NOSPACE_LEVEL 4
#define LOG_ERR_NOSPACE_WHERE 5

#define LOG_ERR_MSG_GETTIMEOFDAY ("gettimeofday() failed." PPE_LOG_NEWLINE)
#define LOG_ERR_MSG_GMTIME_R ("gmtime_r() failed." PPE_LOG_NEWLINE)
#define LOG_ERR_MSG_SNPRINTF ("snprintf() failed." PPE_LOG_NEWLINE)
#define LOG_ERR_MSG_NOSPACE_TIMESTAMP ("No space for the timestamp tag." PPE_LOG_NEWLINE)
#define LOG_ERR_MSG_NOSPACE_LEVEL ("No space for the level tag." PPE_LOG_NEWLINE)
#define LOG_ERR_MSG_NOSPACE_WHERE ("No space for the where tag." PPE_LOG_NEWLINE)

    static const char * const error_msgs_l[] = {
        LOG_ERR_MSG_GETTIMEOFDAY,
        LOG_ERR_MSG_GMTIME_R,
        LOG_ERR_MSG_SNPRINTF,
        LOG_ERR_MSG_NOSPACE_TIMESTAMP,
        LOG_ERR_MSG_NOSPACE_LEVEL,
        LOG_ERR_MSG_NOSPACE_WHERE
    };
    static const ppe_size error_msgs_size_l[] = {
        sizeof(LOG_ERR_MSG_GETTIMEOFDAY) - 1,
        sizeof(LOG_ERR_MSG_GMTIME_R) - 1,
        sizeof(LOG_ERR_MSG_SNPRINTF) - 1,
        sizeof(LOG_ERR_MSG_NOSPACE_TIMESTAMP) - 1,
        sizeof(LOG_ERR_MSG_NOSPACE_LEVEL) - 1,
        sizeof(LOG_ERR_MSG_NOSPACE_WHERE) - 1
    };

    ppe_size size = 0;
    ppe_size len = 0;
    char * pos = NULL;
    struct timeval tv;
    struct tm tm;
    int ret = 0;

    /* Default format of the tags: */
    /* "<Timestamp> [Thread-ID] <Level> [Where] <message>" */

    /* Timestamp Tag */
    {
        ret = gettimeofday(&tv, NULL);
        if (ret < 0) {
            log->write(itf, error_msgs_l[LOG_ERR_GETTIMEOFDAY], error_msgs_size_l[LOG_ERR_GETTIMEOFDAY]);
            return 0;
        }
        if (! gmtime_r(&tv.tv_sec, &tm)) {
            log->write(itf, error_msgs_l[LOG_ERR_GMTIME_R], error_msgs_size_l[LOG_ERR_GMTIME_R]);
            return 0;
        }

        if (log->flags & PPE_LOG_MILLISECOND) {
            ret = snprintf(log->buf, log->buf_cap, "%04d-%02d-%02d %02d:%02d:%02d.%03d ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, (int)(tv.tv_usec / 1000));
        } else {
            ret = snprintf(log->buf, log->buf_cap, "%04d-%02d-%02d %02d:%02d:%02d ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        }
        if (ret < 0) {
            log->write(itf, error_msgs_l[LOG_ERR_SNPRINTF], error_msgs_size_l[LOG_ERR_SNPRINTF]);
            return 0;
        } else if (ret >= log->buf_cap) {
            log->write(itf, error_msgs_l[LOG_ERR_NOSPACE_TIMESTAMP], error_msgs_size_l[LOG_ERR_NOSPACE_TIMESTAMP]);
            return 0;
        }
        size += ret;
    }

    /* TODO: Thread ID Tag */

    /* Level Tag */
    {
        if (size + level_tags_size_l[level] > log->buf_cap - 1) {
            if (! log->write(itf, log->buf, size)) return 0;
            log->write(itf, error_msgs_l[3], error_msgs_size_l[3]);
            return 0;
        }

        memcpy(log->buf + size, level_tags_l[level], level_tags_size_l[level]);
        size += level_tags_size_l[level];
        log->buf[size++] = ' ';
    }

    /* Where Tag */
    if (where) {
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
        if (size + len > log->buf_cap - 1) {
            if (! log->write(itf, log->buf, size)) return 0;
            log->write(itf, error_msgs_l[4], error_msgs_size_l[4]);
            return 0;
        }

        memcpy(log->buf + size, where, len);
        size += len;
        log->buf[size++] = ' ';
    }

    return size;

#undef LOG_ERR_GETTIMEOFDAY
#undef LOG_ERR_GMTIME_R
#undef LOG_ERR_SNPRINTF
#undef LOG_ERR_NOSPACE_TIMESTAMP
#undef LOG_ERR_NOSPACE_LEVEL
#undef LOG_ERR_NOSPACE_WHERE

#undef LOG_ERR_MSG_GETTIMEOFDAY
#undef LOG_ERR_MSG_GMTIME_R
#undef LOG_ERR_MSG_SNPRINTF
#undef LOG_ERR_MSG_NOSPACE_TIMESTAMP
#undef LOG_ERR_MSG_NOSPACE_LEVEL
#undef LOG_ERR_MSG_NOSPACE_WHERE

#undef LOG_DEBUG_TAG
#undef LOG_INFO_TAG
#undef LOG_WARN_TAG
#undef LOG_ERROR_TAG
#undef LOG_FATAL
}

/* -- Create & Destroy -- */

PPE_API ppe_log ppe_log_create(ppe_size buf_cap, ppe_log_level threshold, ppe_uint flags, ppe_log_write_vfn write, ppe_log_flush_vfn flush)
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
    if (log && log != log_stderr_logger_s) free(log);
}

/* -- Property -- */

PPE_API ppe_uint ppe_log_set_flags(ppe_log restrict log, ppe_uint flags)
{
    ppe_uint last = 0;

    assert(log);

    last = log->flags;
    log->flags = flags;
    return last;
}

PPE_API ppe_uint ppe_log_get_flags(ppe_log restrict log)
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

PPE_API ppe_log_itf ppe_log_get_stderr_logger(void)
{
    return &log_stderr_logger_s;
}

PPE_API ppe_log_itf ppe_log_set_global_default(ppe_log_itf restrict itf)
{
    ppe_log_itf last = log_default_logger_s;
    log_default_logger_s = itf;
    return last;
}

PPE_API ppe_log_itf ppe_log_get_global_default(void)
{
    return log_default_logger_s;
}

/* -- Virtual  -- */

PPE_API void ppe_log_write_to(ppe_log_itf restrict itf, const char * restrict where, ppe_log_level level, const char * msg, ppe_size msg_size)
{
    ppe_log log = NULL;
    ppe_size size = 0;

    assert(PPE_LOG_DEBUG <= level && level <= PPE_LOG_FATAL);
    assert(msg);

    if (! itf) itf = ppe_log_get_global_default();
    log = *itf;

    if (level < log->threshold) return;

    size = log_format_tags(itf, log, level, where);
    if (size > 0 && ! log->write(itf, log->buf, size)) return;

    if (! log->write(itf, msg, msg_size > 0 ? msg_size : strlen(msg))) return;
    if (! log->write(itf, log_newline_s, log_newline_size_s)) return;

    log->flush(itf);
}

PPE_API void ppe_log_vprintf_to(ppe_log_itf restrict itf, const char * restrict where, ppe_log_level level, const char * restrict fmt, va_list args)
{
#define LOG_ERR_VSNPRINTF 0
#define LOG_ERR_MSG_VSNPRINTF ("vsnprintf() failed." PPE_LOG_NEWLINE)
    static const char * const error_msgs_l[] = {
        LOG_ERR_MSG_VSNPRINTF
    };
    static const ppe_size error_msgs_size_l[] = {
        sizeof(LOG_ERR_MSG_VSNPRINTF) - 1
    };

    ppe_log log = NULL;
    ppe_size size = 0;
    int ret = 0;

    assert(PPE_LOG_DEBUG <= level && level <= PPE_LOG_FATAL);
    assert(fmt && strlen(fmt) > 0 && strchr(fmt, '%'));

    if (! itf) itf = ppe_log_get_global_default();
    log = *itf;

    if (level < log->threshold) return;
    
    size = log_format_tags(itf, log, level, where);

    /* Format the message. */
    ret = vsnprintf(log->buf + size, log->buf_cap - size, fmt, args);
    if (ret < 0) {
        if (size > 0 && ! log->write(itf, log->buf, size)) return;
        if (! log->write(itf, error_msgs_l[0], error_msgs_size_l[0])) return;
        log->flush(itf);
        return;
    } else if (ret == 0) {
        /* No message to write. */
        return;
    } else if (ret >= log->buf_cap - size ) {
        /* Exceeded the capacity of the remainder space. */
        /* Write the tags and then reformat the message. */
        if (! log->write(itf, log->buf, size)) return;

        size = 0;
        ret = vsnprintf(log->buf, log->buf_cap, fmt, args);
        if (ret < 0) {
            if (size > 0 && ! log->write(itf, log->buf, size)) return;
            if (! log->write(itf, error_msgs_l[0], error_msgs_size_l[0])) return;
            log->flush(itf);
            return;
        }
        if (ret > log->buf_cap) {
            /* Exceeded the capacity of the remainder space. */
            /* Truncate the message. */
            ret = log->buf_cap;
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
    if (! log->write(itf, log_newline_s, log_newline_size_s)) return;
    log->flush(itf);

#undef LOG_ERR_VSNPRINTF
#undef LOG_ERR_MSG_VSNPRINTF
}

#ifdef __cplusplus
}
#endif

