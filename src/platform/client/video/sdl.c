#include "client/blit.h"
#include "client/palette.h"
#include "client/video.h"
#include "skele.h"
#include <SDL3/SDL.h>
#include <stdint.h>
#include <stk/stk_log.h>
#include <string.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;
static SDL_Palette *sdl_pal = NULL;
static uint16_t vid_w = 0;
static uint16_t vid_h = 0;
static uint32_t vid_total = 0;
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

	renderer = SDL_CreateRenderer(window, NULL);
	if (!renderer) {
		stk_log(STK_LOG_ERROR, "video: SDL_CreateRenderer failed: %s",
			SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return SKELE_INIT_FAILURE;
	}

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_INDEX8,
				    SDL_TEXTUREACCESS_STREAMING, vid_w, vid_h);
	if (!texture) {
		stk_log(STK_LOG_ERROR, "video: SDL_CreateTexture failed: %s",
			SDL_GetError());
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return SKELE_INIT_FAILURE;
	}

	sdl_pal = SDL_CreatePalette(SKELE_PALETTE_COLORS);
	if (!sdl_pal) {
		stk_log(STK_LOG_ERROR, "video: SDL_CreatePalette failed: %s",
			SDL_GetError());
		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return SKELE_INIT_FAILURE;
	}

	SDL_SetTexturePalette(texture, sdl_pal);

	{
		SDL_Color black[SKELE_PALETTE_COLORS];
		uint16_t i;
		memset(black, 0, sizeof(black));
		for (i = 0; i < SKELE_PALETTE_COLORS; i++)
			black[i].a = 255;
		SDL_SetPaletteColors(sdl_pal, black, 0, SKELE_PALETTE_COLORS);
	}

	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
	SDL_SetRenderVSync(renderer, 1);
	SDL_ShowWindow(window);
	return SKELE_INIT_SUCCESS;
}

void skele_video_shutdown(void)
{
	if (sdl_pal)
		SDL_DestroyPalette(sdl_pal);
	if (texture)
		SDL_DestroyTexture(texture);
	if (renderer)
		SDL_DestroyRenderer(renderer);
	if (window)
		SDL_DestroyWindow(window);
	SDL_Quit();
	sdl_pal = NULL;
	texture = NULL;
	renderer = NULL;
	window = NULL;
	vid_w = 0;
	vid_h = 0;
	vid_total = 0;
	cur_scale = 1;
	fullscreen = 0;
}

void skele_video_present(void) { SDL_RenderPresent(renderer); }
void skele_video_toggle_fullscreen(void) { toggle_fullscreen(); }
void skele_video_cycle_scale(void) { cycle_scale(); }

void skele_video_set_mouse_grab(uint8_t grab)
{
	SDL_SetWindowMouseGrab(window, grab ? true : false);
	SDL_SetWindowRelativeMouseMode(window, grab ? true : false);
}

void skele_palette_set(skele_palette_t pal)
{
	SDL_Color colors[SKELE_PALETTE_COLORS];
	uint16_t i;
	uint32_t c;

	for (i = 0; i < SKELE_PALETTE_COLORS; i++) {
		c = pal[i];
		colors[i].r = (uint8_t)(c >> 16);
		colors[i].g = (uint8_t)(c >> 8);
		colors[i].b = (uint8_t)(c);
		colors[i].a = 255;
	}
	SDL_SetPaletteColors(sdl_pal, colors, 0, SKELE_PALETTE_COLORS);
}

void skele_palette_set_index(uint8_t index, uint32_t color)
{
	SDL_Color c;
	c.r = (uint8_t)(color >> 16);
	c.g = (uint8_t)(color >> 8);
	c.b = (uint8_t)(color);
	c.a = 255;
	SDL_SetPaletteColors(sdl_pal, &c, index, 1);
}

void skele_video_blit(uint8_t *pixels)
{
	SDL_UpdateTexture(texture, NULL, pixels, vid_w);
	SDL_RenderTexture(renderer, texture, NULL, NULL);
}

void skele_video_set_title(const char *title)
{
	SDL_SetWindowTitle(window, title);
}
