#ifndef SKELE_VIDEO_H
#define SKELE_VIDEO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SKELE_VIDEO_FULLSCREEN 0x01
#define SKELE_VIDEO_BORDERLESS 0x02
#define SKELE_VIDEO_RESIZABLE 0x04
#define SKELE_VIDEO_HIGHDPI 0x08

#define SKELE_DEFAULT_RENDER_WIDTH 320
#define SKELE_DEFAULT_RENDER_HEIGHT 200

typedef struct {
	uint16_t render_width;
	uint16_t render_height;
	uint16_t window_width;
	uint16_t window_height;
	uint8_t flags;
} skele_video_config_t;

uint8_t skele_video_init(skele_video_config_t cfg);
void skele_video_shutdown(void);
void skele_video_present(void);
void skele_video_set_title(const char *title);
void skele_video_toggle_fullscreen(void);
void skele_video_cycle_scale(void);
void skele_video_set_mouse_grab(uint8_t grab);

#ifdef __cplusplus
}
#endif

#endif /* SKELE_VIDEO_H */
