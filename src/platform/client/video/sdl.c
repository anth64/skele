#include "client/blit.h"
#include "client/palette.h"
#include "client/video.h"
#include "skele.h"
#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdlib.h>
#include <stk/stk_log.h>
#include <string.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;
static uint32_t palette[SKELE_PALETTE_COLORS];
static uint32_t *rgba_buf = NULL;
static uint16_t vid_w = 0;
static uint16_t vid_h = 0;
static uint32_t vid_total = 0;
static uint8_t cur_scale = 1;
static uint8_t is_fullscreen = 0;
static skele_fullscreen_kind_t last_fullscreen_kind =
    SKELE_FULLSCREEN_EXCLUSIVE;

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

static void apply_fullscreen_kind(skele_fullscreen_kind_t kind)
{
	if (kind == SKELE_FULLSCREEN_EXCLUSIVE) {
		SDL_DisplayID display;
		const SDL_DisplayMode *mode;
		display = SDL_GetDisplayForWindow(window);
		mode = SDL_GetCurrentDisplayMode(display);
		SDL_SetWindowFullscreenMode(window, mode);
	} else {
		SDL_SetWindowFullscreenMode(window, NULL);
	}
}

static void cycle_scale(void)
{
	uint8_t next;
	if (is_fullscreen)
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

	if (cfg.flags & SKELE_VIDEO_FULLSCREEN)
		flags |= SDL_WINDOW_FULLSCREEN;
	if (cfg.flags & SKELE_VIDEO_BORDERLESS)
		flags |= SDL_WINDOW_BORDERLESS;
	if (cfg.flags & SKELE_VIDEO_RESIZABLE)
		flags |= SDL_WINDOW_RESIZABLE;
	if (cfg.flags & SKELE_VIDEO_HIGHDPI)
		flags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;

	if (cfg.flags & SKELE_VIDEO_FULLSCREEN_EXCLUSIVE)
		last_fullscreen_kind = SKELE_FULLSCREEN_EXCLUSIVE;
	else
		last_fullscreen_kind = SKELE_FULLSCREEN_BORDERLESS;

	display = SDL_GetPrimaryDisplay();
	vid_w =
	    cfg.render_width ? cfg.render_width : SKELE_DEFAULT_RENDER_WIDTH;
	vid_h =
	    cfg.render_height ? cfg.render_height : SKELE_DEFAULT_RENDER_HEIGHT;
	vid_total = (uint32_t)(vid_w * vid_h);

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

	rgba_buf = malloc(vid_total * sizeof(uint32_t));
	if (!rgba_buf) {
		stk_log(STK_LOG_ERROR, "video: out of memory");
		SDL_Quit();
		return SKELE_INIT_FAILURE;
	}
	memset(rgba_buf, 0, vid_total * sizeof(uint32_t));

	window = SDL_CreateWindow("skele", window_w, window_h,
				  flags | SDL_WINDOW_HIDDEN);
	if (!window) {
		stk_log(STK_LOG_ERROR, "video: SDL_CreateWindow failed: %s",
			SDL_GetError());
		free(rgba_buf);
		rgba_buf = NULL;
		SDL_Quit();
		return SKELE_INIT_FAILURE;
	}

	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED_DISPLAY(display),
			      SDL_WINDOWPOS_CENTERED_DISPLAY(display));

	if (cfg.flags & SKELE_VIDEO_FULLSCREEN) {
		apply_fullscreen_kind(last_fullscreen_kind);
		is_fullscreen = 1;
	}

	renderer = SDL_CreateRenderer(window, NULL);
	if (!renderer) {
		stk_log(STK_LOG_ERROR, "video: SDL_CreateRenderer failed: %s",
			SDL_GetError());
		free(rgba_buf);
		rgba_buf = NULL;
		SDL_DestroyWindow(window);
		SDL_Quit();
		return SKELE_INIT_FAILURE;
	}

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_XRGB8888,
				    SDL_TEXTUREACCESS_STREAMING, vid_w, vid_h);
	if (!texture) {
		stk_log(STK_LOG_ERROR, "video: SDL_CreateTexture failed: %s",
			SDL_GetError());
		free(rgba_buf);
		rgba_buf = NULL;
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return SKELE_INIT_FAILURE;
	}

	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
	SDL_SetRenderVSync(renderer, 1);
	memset(palette, 0, sizeof(palette));
	SDL_ShowWindow(window);
	return SKELE_INIT_SUCCESS;
}

void skele_video_shutdown(void)
{
	if (rgba_buf) {
		free(rgba_buf);
		rgba_buf = NULL;
	}
	if (texture) {
		SDL_DestroyTexture(texture);
		texture = NULL;
	}
	if (renderer) {
		SDL_DestroyRenderer(renderer);
		renderer = NULL;
	}
	if (window) {
		SDL_DestroyWindow(window);
		window = NULL;
	}
	SDL_Quit();
	vid_w = 0;
	vid_h = 0;
	vid_total = 0;
	cur_scale = 1;
	is_fullscreen = 0;
}

void skele_video_blit(uint8_t *pixels)
{
	uint32_t i;
	for (i = 0; i < vid_total; i++)
		rgba_buf[i] = palette[pixels[i]];
	SDL_UpdateTexture(texture, NULL, rgba_buf,
			  vid_w * (int)sizeof(uint32_t));
	SDL_RenderTexture(renderer, texture, NULL, NULL);
}

void skele_video_present(void) { SDL_RenderPresent(renderer); }
void skele_video_cycle_scale(void) { cycle_scale(); }

void skele_video_toggle_fullscreen(void)
{
	if (is_fullscreen) {
		SDL_SetWindowFullscreen(window, false);
		is_fullscreen = 0;
	} else {
		apply_fullscreen_kind(last_fullscreen_kind);
		SDL_SetWindowFullscreen(window, true);
		is_fullscreen = 1;
	}
}

void skele_video_set_fullscreen_kind(skele_fullscreen_kind_t kind)
{
	last_fullscreen_kind = kind;
	if (is_fullscreen)
		apply_fullscreen_kind(kind);
}

void skele_video_set_mouse_grab(uint8_t grab)
{
	SDL_SetWindowMouseGrab(window, grab ? true : false);
	SDL_SetWindowRelativeMouseMode(window, grab ? true : false);
}

void skele_video_set_title(const char *title)
{
	SDL_SetWindowTitle(window, title);
}

void skele_palette_set(skele_palette_t pal)
{
	uint16_t i;
	for (i = 0; i < SKELE_PALETTE_COLORS; i++)
		palette[i] = pal[i] | 0xFF000000;
}

void skele_palette_set_index(uint8_t index, uint32_t color)
{
	palette[index] = color | 0xFF000000;
}
