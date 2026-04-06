#include "clock.h"
#include <windows.h>

static void (*signal_cb)(void) = NULL;
static LARGE_INTEGER perf_freq;

static BOOL WINAPI handle_ctrl(DWORD type)
{
	(void)type;
	if (signal_cb)
		signal_cb();
	return TRUE;
}

void skele_clock_init(void (*on_signal)(void))
{
	signal_cb = on_signal;
	QueryPerformanceFrequency(&perf_freq);
	SetConsoleCtrlHandler(handle_ctrl, TRUE);
}

uint64_t skele_time_ns(void)
{
	LARGE_INTEGER cnt;
	QueryPerformanceCounter(&cnt);
	return (uint64_t)(cnt.QuadPart * 1000000000ULL / perf_freq.QuadPart);
}

void skele_sleep_ns(uint64_t ns)
{
	HANDLE timer = CreateWaitableTimer(NULL, TRUE, NULL);
	LARGE_INTEGER li;
	li.QuadPart = -(LONGLONG)(ns / 100);
	SetWaitableTimer(timer, &li, 0, NULL, NULL, FALSE);
	WaitForSingleObject(timer, INFINITE);
	CloseHandle(timer);
}
