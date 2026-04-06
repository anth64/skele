#define _POSIX_C_SOURCE 199309L
#include "clock.h"
#include <signal.h>
#include <time.h>

static void (*signal_cb)(void) = NULL;

static void handle_signal(int sig)
{
	(void)sig;
	if (signal_cb)
		signal_cb();
}

void skele_clock_init(void (*on_signal)(void))
{
	signal_cb = on_signal;
	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);
}

uint64_t skele_time_ns(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (uint64_t)(ts.tv_sec * 1000000000ULL + ts.tv_nsec);
}

void skele_sleep_ns(uint64_t ns)
{
	struct timespec ts;
	ts.tv_sec = (time_t)(ns / 1000000000ULL);
	ts.tv_nsec = (long)(ns % 1000000000ULL);
	nanosleep(&ts, NULL);
}
