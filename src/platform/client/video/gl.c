#include "client/video.h"
#include "skele.h"
#include <SDL3/SDL.h>
#include <stk/stk_log.h>

static SDL_Window *window = NULL;
static SDL_GLContext gl_ctx = NULL;
static uint16_t vid_w = 0;
static uint16_t vid_h = 0;
static uint8_t cur_scale = 1;
static uint8_t fullscreen = 0;

static uint8_t max_scale(uint16_t rw, uint16_t rh)
{
	SDL_DisplayID display;
	const SDL_DisplayMode *mode;
	uint16_t aw, ah;
	uint8_t scale;

	display =
	    window ? SDL_GetDisplayForWindow(window) : SDL_GetPrimaryDisplay();
	mode = SDL_GetCurrentDisplayMode(display);
	if (!mode)
		return 1;

	aw = (uint16_t)((mode->w * 3) / 4);
	ah = (uint16_t)((mode->h * 3) / 4);
	scale = 1;
	while ((rw * (scale + 1)) <= aw && (rh * (scale + 1)) <= ah)
		scale++;

	return scale;
}

static void set_scale(uint8_t scale)
{
	SDL_DisplayID display;
	cur_scale = scale;
	display = SDL_GetDisplayForWindow(window);
	SDL_SetWindowSize(window, (int)(vid_w * scale), (int)(vid_h * scale));
	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED_DISPLAY(display),
			      SDL_WINDOWPOS_CENTERED_DISPLAY(display));
}

static void toggle_fullscreen(void)
{
	fullscreen = !fullscreen;
	SDL_SetWindowFullscreen(window, fullscreen);
}

static void cycle_scale(void)
{
	uint8_t next;
	if (fullscreen)
		return;
	next = cur_scale + 1;
	if (next > max_scale(vid_w, vid_h))
		next = 1;
	set_scale(next);
}

uint8_t skele_video_init(skele_video_config_t cfg)
{
	SDL_DisplayID display;
	SDL_WindowFlags flags = 0;
	const SDL_DisplayMode *mode;
	uint16_t window_w, window_h, aw, ah;

	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
		stk_log(STK_LOG_ERROR, "video: SDL_Init failed: %s",
			SDL_GetError());
		return SKELE_INIT_FAILURE;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GL_MAJOR);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GL_MINOR);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
			    SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	flags |= SDL_WINDOW_OPENGL;
	if (cfg.flags & SKELE_VIDEO_FULLSCREEN)
		flags |= SDL_WINDOW_FULLSCREEN;
	if (cfg.flags & SKELE_VIDEO_BORDERLESS)
		flags |= SDL_WINDOW_BORDERLESS;
	if (cfg.flags & SKELE_VIDEO_RESIZABLE)
		flags |= SDL_WINDOW_RESIZABLE;
	if (cfg.flags & SKELE_VIDEO_HIGHDPI)
		flags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;

	display = SDL_GetPrimaryDisplay();
	vid_w =
	    cfg.render_width ? cfg.render_width : SKELE_DEFAULT_RENDER_WIDTH;
	vid_h =
	    cfg.render_height ? cfg.render_height : SKELE_DEFAULT_RENDER_HEIGHT;

	if (cfg.window_width && cfg.window_height) {
		window_w = cfg.window_width;
		window_h = cfg.window_height;
		cur_scale = 1;
	} else {
		mode = SDL_GetCurrentDisplayMode(display);
		if (!mode) {
			stk_log(STK_LOG_ERROR,
				"video: SDL_GetCurrentDisplayMode failed: %s",
				SDL_GetError());
			SDL_Quit();
			return SKELE_INIT_FAILURE;
		}

		aw = (uint16_t)((mode->w * 3) / 4);
		ah = (uint16_t)((mode->h * 3) / 4);
		cur_scale = 1;
		while ((vid_w * (cur_scale + 1)) <= aw &&
		       (vid_h * (cur_scale + 1)) <= ah)
			cur_scale++;

		window_w = vid_w * cur_scale;
		window_h = vid_h * cur_scale;
	}

	window = SDL_CreateWindow("skele", window_w, window_h,
				  flags | SDL_WINDOW_HIDDEN);
	if (!window) {
		stk_log(STK_LOG_ERROR, "video: SDL_CreateWindow failed: %s",
			SDL_GetError());
		SDL_Quit();
		return SKELE_INIT_FAILURE;
	}

	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED_DISPLAY(display),
			      SDL_WINDOWPOS_CENTERED_DISPLAY(display));

	if (cfg.flags & SKELE_VIDEO_FULLSCREEN)
		fullscreen = 1;

	gl_ctx = SDL_GL_CreateContext(window);
	if (!gl_ctx) {
		stk_log(STK_LOG_ERROR, "video: SDL_GL_CreateContext failed: %s",
			SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return SKELE_INIT_FAILURE;
	}

	SDL_GL_SetSwapInterval(1);
	SDL_ShowWindow(window);
	return SKELE_INIT_SUCCESS;
}

void skele_video_shutdown(void)
{
	if (gl_ctx)
		SDL_GL_DestroyContext(gl_ctx);
	if (window)
		SDL_DestroyWindow(window);
	SDL_Quit();
	gl_ctx = NULL;
	window = NULL;
	vid_w = 0;
	vid_h = 0;
	cur_scale = 1;
	fullscreen = 0;
}

void skele_video_present(void) { SDL_GL_SwapWindow(window); }
void skele_video_toggle_fullscreen(void) { toggle_fullscreen(); }
void skele_video_cycle_scale(void) { cycle_scale(); }

void skele_video_set_mouse_grab(uint8_t grab)
{
	SDL_SetWindowMouseGrab(window, grab ? true : false);
	SDL_SetWindowRelativeMouseMode(window, grab ? true : false);
}

void skele_video_set_title(const char *title)
{
	SDL_SetWindowTitle(window, title);
}
