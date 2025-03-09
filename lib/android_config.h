#pragma once

#include <pthread.h>
#include <signal.h>

// Signal handler that does nothing but indicates receipt
static void signal_handler(int signo) {
    /* Nothing to do */
}

// Macro to set up SIG signal handler to do nothing
#define SETUP_SIG_HANDLER(sig)                                      \
    do {                                                            \
        struct sigaction sa;                                        \
        sa.sa_handler = signal_handler;                             \
        sa.sa_flags = 0;                                            \
        sigemptyset(&sa.sa_mask);                                   \
        sigaction(sig, &sa, NULL);                                  \
    } while (0)

// Macros for signal handling
#define ENABLE_SIGNAL(sig)                          \
    do {                                            \
        SETUP_SIG_HANDLER(sig);  /* Ensure SIG is handled by empty_signal_handler */ \
        sigset_t set;                               \
        sigemptyset(&set);                          \
        sigaddset(&set, sig);                       \
        pthread_sigmask(SIG_UNBLOCK, &set, NULL);   \
    } while (0)

#define DISABLE_SIGNAL(sig)                         \
    do {                                            \
        sigset_t set;                               \
        sigemptyset(&set);                          \
        sigaddset(&set, sig);                       \
        pthread_sigmask(SIG_BLOCK, &set, NULL);     \
    } while (0)

// Macros to simulate pthread_setcancelstate and pthread_cancel
#define PTHREAD_CANCEL_ENABLE 1
#define PTHREAD_CANCEL_DISABLE 0

#define pthread_setcancelstate(state, oldstate)                             \
    do {                                                                    \
        static __thread int thread_cancel_state = PTHREAD_CANCEL_ENABLE;    \
        if (oldstate != NULL) {                                             \
            *(int*)oldstate = thread_cancel_state;                          \
        }                                                                   \
        if (state == PTHREAD_CANCEL_ENABLE) {                               \
            thread_cancel_state = PTHREAD_CANCEL_ENABLE;                    \
            ENABLE_SIGNAL(SIGUSR1);                                         \
        } else if (state == PTHREAD_CANCEL_DISABLE) {                       \
            thread_cancel_state = PTHREAD_CANCEL_DISABLE;                   \
            DISABLE_SIGNAL(SIGUSR1);                                        \
        }                                                                   \
    } while (0)

#define pthread_cancel(thread)                                              \
    do {                                                                    \
        pthread_kill(thread, SIGUSR1);                                      \
    } while (0)
