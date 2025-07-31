
#include "mimir/log.h"
#include "spdlog/spdlog.h"

#include "cstdarg"
#include "cstdio"

#ifdef MIMIR_ENABLE_LOGGING
  #include "spdlog/sinks/stdout_color_sinks.h"
  #include "spdlog/logger.h"
  #include "spdlog/spdlog.h"
#endif

// maximum formatted log line
static constexpr int LOG_BUF_SIZE = 1024;

#ifdef MIMIR_ENABLE_LOGGING
// lazy‐initialized spdlog logger
static std::shared_ptr<spdlog::logger> get_logger() {
    static std::shared_ptr<spdlog::logger> logger = [] {
        auto lg = spdlog::stdout_color_mt("ccask");
        lg->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
        return lg;
    }();
    return logger;
}

// internal helper to format a va_list into a stack buffer
static void vlog_to_buffer(char *buf, size_t buf_size, const char *fmt, va_list ap) {
    vsnprintf(buf, buf_size, fmt, ap);
}

// Macros to define each log level function
#define DEFINE_LOG_FN(level_name, spd_level)          \
extern "C" void log_##level_name(const char *fmt, ...) { \
    va_list ap;                                       \
    va_start(ap, fmt);                                \
    char buf[LOG_BUF_SIZE];                           \
    vlog_to_buffer(buf, sizeof(buf), fmt, ap);        \
    va_end(ap);                                       \
    auto lg = get_logger();                           \
    lg->spd_level(buf);                               \
}

DEFINE_LOG_FN(trace,   trace)
DEFINE_LOG_FN(debug,   debug)
DEFINE_LOG_FN(info,    info)
DEFINE_LOG_FN(warn,    warn)
DEFINE_LOG_FN(error,   error)
DEFINE_LOG_FN(fatal,   critical)  // map `log_fatal` → spdlog::critical

#undef DEFINE_LOG_FN

#else

extern "C" {
    void log_trace(const char *fmt, ...) {}
    void log_debug(const char *fmt, ...) {}
    void log_info(const char *fmt, ...) {}
    void log_warn(const char *fmt, ...) {}
    void log_error(const char *fmt, ...) {}
    void log_fatal(const char *fmt, ...) {}
}

#endif
