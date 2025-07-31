
#include "mimir/server.h"
#include "mimir/log.h"
#include "mimir/status.h"

#include "stdlib.h"
#include "stdbool.h"
#include "stdatomic.h"
#include "pthread.h"
#include "string.h"
#include "unistd.h"
#include "netdb.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "arpa/inet.h"

#define PORT "8080"
#define BACKLOG 10

static struct {
    int listen_fd;
    pthread_t listen_thr;
    atomic_bool shutdown;
} server_state;

// forward-declaration
static void* listen_thread(void *arg);

mimir_status_e mimir_server_init() {
    server_state.listen_fd = -1;
    server_state.listen_thr = -1;
    atomic_store_explicit(&server_state.shutdown, false, memory_order_release);

    struct addrinfo hints;
    struct addrinfo *servInfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status;
    if ((status = getaddrinfo(NULL, PORT, &hints, &servInfo)) != 0) {
        log_fatal("Failed to start server: %s", gai_strerror(status));
        mimir_errno = ERR_SERVER_GET_ADDRINFO_FAILED;
        return MIMIR_FAIL;
    }

    int sockfd, yes = 1;
    struct addrinfo *p;
    for(p = servInfo; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd < 0) {
            continue;
        }

        int res = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        if (res != 0) {
            close(sockfd);
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) != 0) {
            close(sockfd);
            continue;
        }

        break;
    }

    freeaddrinfo(servInfo);

    if (p == NULL)  {
        log_fatal("Failed to bind server socket to address");
        mimir_errno = ERR_SERVER_BIND_FAILED;
        return MIMIR_FAIL;
    }

    if (listen(sockfd, BACKLOG) != 0) {
        close(sockfd);
        log_fatal("Failed to start listening for connections");
        mimir_errno = ERR_SERVER_LISTEN_FAILED;
        return MIMIR_FAIL;
    }

    pthread_t thr;
    if (pthread_create(&thr, NULL, listen_thread, NULL) != 0) {
        close(sockfd);
        log_fatal("Failed to start listener thread");
        mimir_errno = ERR_SERVER_LISTEN_THREAD_FAILED;
        return MIMIR_FAIL;
    }

    server_state.listen_thr = thr;
    server_state.listen_fd = sockfd;
    return MIMIR_OK;
}

void mimir_server_shutdown() {
    atomic_store_explicit(&server_state.shutdown, true, memory_order_release);
    pthread_join(server_state.listen_thr, NULL);
    close(server_state.listen_fd);
}

static void* listen_thread(void *arg) {
    (void)arg;

    while (!atomic_load_explicit(&server_state.shutdown, memory_order_acquire)) {
        usleep(1000);
    }

    pthread_exit(NULL);
    return NULL;
}
