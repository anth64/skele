#ifndef SKELE_INPUT_H
#define SKELE_INPUT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	SKELE_KEY_UNKNOWN = 0,

	SKELE_KEY_A,
	SKELE_KEY_B,
	SKELE_KEY_C,
	SKELE_KEY_D,
	SKELE_KEY_E,
	SKELE_KEY_F,
	SKELE_KEY_G,
	SKELE_KEY_H,
	SKELE_KEY_I,
	SKELE_KEY_J,
	SKELE_KEY_K,
	SKELE_KEY_L,
	SKELE_KEY_M,
	SKELE_KEY_N,
	SKELE_KEY_O,
	SKELE_KEY_P,
	SKELE_KEY_Q,
	SKELE_KEY_R,
	SKELE_KEY_S,
	SKELE_KEY_T,
	SKELE_KEY_U,
	SKELE_KEY_V,
	SKELE_KEY_W,
	SKELE_KEY_X,
	SKELE_KEY_Y,
	SKELE_KEY_Z,

	SKELE_KEY_0,
	SKELE_KEY_1,
	SKELE_KEY_2,
	SKELE_KEY_3,
	SKELE_KEY_4,
	SKELE_KEY_5,
	SKELE_KEY_6,
	SKELE_KEY_7,
	SKELE_KEY_8,
	SKELE_KEY_9,

	SKELE_KEY_F1,
	SKELE_KEY_F2,
	SKELE_KEY_F3,
	SKELE_KEY_F4,
	SKELE_KEY_F5,
	SKELE_KEY_F6,
	SKELE_KEY_F7,
	SKELE_KEY_F8,
	SKELE_KEY_F9,
	SKELE_KEY_F10,
	SKELE_KEY_F11,
	SKELE_KEY_F12,

	SKELE_KEY_SPACE,
	SKELE_KEY_ENTER,
	SKELE_KEY_ESCAPE,
	SKELE_KEY_TAB,
	SKELE_KEY_BACKSPACE,
	SKELE_KEY_LSHIFT,
	SKELE_KEY_RSHIFT,
	SKELE_KEY_LCTRL,
	SKELE_KEY_RCTRL,
	SKELE_KEY_LALT,
	SKELE_KEY_RALT,

	SKELE_KEY_UP,
	SKELE_KEY_DOWN,
	SKELE_KEY_LEFT,
	SKELE_KEY_RIGHT,

	SKELE_KEY_COUNT
} skele_key_t;

typedef enum {
	SKELE_PAD_A,
	SKELE_PAD_B,
	SKELE_PAD_X,
	SKELE_PAD_Y,
	SKELE_PAD_LB,
	SKELE_PAD_RB,
	SKELE_PAD_LT,
	SKELE_PAD_RT,
	SKELE_PAD_LSTICK,
	SKELE_PAD_RSTICK,
	SKELE_PAD_START,
	SKELE_PAD_BACK,
	SKELE_PAD_DPAD_UP,
	SKELE_PAD_DPAD_DOWN,
	SKELE_PAD_DPAD_LEFT,
	SKELE_PAD_DPAD_RIGHT,
	SKELE_PAD_BUTTON_COUNT
} skele_pad_button_t;

typedef enum {
	SKELE_PAD_AXIS_LX,
	SKELE_PAD_AXIS_LY,
	SKELE_PAD_AXIS_RX,
	SKELE_PAD_AXIS_RY,
	SKELE_PAD_AXIS_LT,
	SKELE_PAD_AXIS_RT,
	SKELE_PAD_AXIS_COUNT
} skele_pad_axis_t;

#define SKELE_MAX_PADS 4

uint8_t skele_input_poll(void);

uint8_t skele_key_down(skele_key_t key);
uint8_t skele_key_held(skele_key_t key);

uint8_t skele_pad_connected(uint8_t pad);
uint8_t skele_pad_button_down(uint8_t pad, skele_pad_button_t btn);
uint8_t skele_pad_button_held(uint8_t pad, skele_pad_button_t btn);
float skele_pad_axis(uint8_t pad, skele_pad_axis_t axis);

void skele_mouse_delta(int32_t *dx, int32_t *dy);

#ifdef __cplusplus
}
#endif

#endif /* SKELE_INPUT_H */
