#include "client/input.h"
#include "client/video.h"
#include "clock.h"
#include "skele.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <stk/stk.h>

static uint8_t running = 1;

static void on_signal(void) { running = 0; }

int main(int argc, char *argv[])
{
	skele_video_config_t video_cfg;
	uint64_t last, now, elapsed, accum = 0;

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

	last = skele_time_ns();

	while (running) {
		if (!skele_input_poll())
			break;

		stk_poll();

		now = skele_time_ns();
		elapsed = now - last;
		accum += elapsed;
		last = now;

		while (accum >= skele_tick_ns) {
			skele_tick();
			accum -= skele_tick_ns;
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		skele_video_present();
	}

	skele_video_shutdown();
	skele_stk_teardown();
	skele_shutdown();
	return 0;
}
