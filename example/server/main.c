#include "clock.h"
#include "skele.h"
#include <stk/stk.h>

static uint8_t running = 1;

static void on_signal(void) { running = 0; }

int main(int argc, char *argv[])
{
	uint64_t tick_start;
	uint64_t elapsed;

	skele_clock_init(on_signal);

	if (skele_stk_setup() != SKELE_INIT_SUCCESS)
		return 1;

	if (skele_init() != SKELE_INIT_SUCCESS) {
		skele_stk_teardown();
		return 1;
	}

	while (running) {
		tick_start = skele_time_ns();

		stk_poll();
		skele_tick();

		elapsed = skele_time_ns() - tick_start;
		if (elapsed < skele_tick_ns)
			skele_sleep_ns(skele_tick_ns - elapsed);
	}

	skele_stk_teardown();
	skele_shutdown();
	return 0;
}
