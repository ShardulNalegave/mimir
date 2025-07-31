
#ifndef MIMIR_STATUS_H
#define MIMIR_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum mimir_error {
    ERR_UNKNOWN = 0,

    ERR_SERVER_GET_ADDRINFO_FAILED      = 1,
    ERR_SERVER_BIND_FAILED              = 2,
    ERR_SERVER_LISTEN_FAILED            = 3,
    ERR_SERVER_LISTEN_THREAD_FAILED     = 4,
} mimir_error_e;

typedef enum mimir_status {
    MIMIR_OK        = 0,
    MIMIR_FAIL      = -1,
    MIMIR_RETRY     = -2,
} mimir_status_e;

#define MIMIR_ATTEMPT(max_attempts, out_var, expr)              \
    do {                                                        \
        int _ccask_retry_cnt = 0;                               \
        for (; _ccask_retry_cnt < (max_attempts);               \
            ++_ccask_retry_cnt)                                 \
        {                                                       \
        (out_var) = (expr);                                     \
        if (out_var != CCASK_RETRY)                             \
            break;                                              \
        }                                                       \
    } while (0)

extern mimir_error_e mimir_errno;

#ifdef __cplusplus
}
#endif

#endif