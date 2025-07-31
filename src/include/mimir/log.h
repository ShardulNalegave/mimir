
#ifndef MIMIR_LOG_H
#define MIMIR_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

void log_trace(const char *fmt, ...);
void log_debug(const char *fmt, ...);
void log_info(const char *fmt, ...);
void log_warn(const char *fmt, ...);
void log_error(const char *fmt, ...);
void log_fatal(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif