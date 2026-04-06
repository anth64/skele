#include "client/blit.h"
#include "client/input.h"
#include "client/video.h"
#include "clock.h"
#include "skele.h"
#include <stdlib.h>
#include <stk/stk.h>
#include <string.h>

static uint8_t running = 1;

static void on_signal(void) { running = 0; }

int main(int argc, char *argv[])
{
	skele_video_config_t video_cfg;
	uint64_t last, now, elapsed;
	uint8_t *pixels;
	uint32_t total;

	(void)argc;
	(void)argv;

	skele_clock_init(on_signal);

	if (skele_stk_setup() != SKELE_INIT_SUCCESS)
		return 1;

	if (skele_init() != SKELE_INIT_SUCCESS) {
		skele_stk_teardown();
		return 1;
	}

	video_cfg.render_width = SKELE_DEFAULT_RENDER_WIDTH;
	video_cfg.render_height = SKELE_DEFAULT_RENDER_HEIGHT;
	video_cfg.window_width = 0;
	video_cfg.window_height = 0;
	video_cfg.flags = 0;

	if (skele_video_init(video_cfg) != SKELE_INIT_SUCCESS) {
		skele_stk_teardown();
		skele_shutdown();
		return 1;
	}

	total = (uint32_t)(video_cfg.render_width * video_cfg.render_height);
	pixels = malloc(total);
	if (!pixels) {
		skele_video_shutdown();
		skele_stk_teardown();
		skele_shutdown();
		return 1;
	}

	memset(pixels, 0, total);

	last = skele_time_ns();

	while (running) {
		if (!skele_input_poll())
			break;

		stk_poll();

		now = skele_time_ns();
		elapsed = now - last;

		if (elapsed >= skele_tick_ns) {
			skele_tick();
			last = now;
		}

		skele_video_blit(pixels);
		skele_video_present();
	}

	free(pixels);
	skele_video_shutdown();
	skele_stk_teardown();
	skele_shutdown();
	return 0;
}
