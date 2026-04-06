#include "client/input.h"
#include "client/video.h"
#include <SDL3/SDL.h>
#include <string.h>

static uint8_t key_held[SKELE_KEY_COUNT];
static uint8_t key_down[SKELE_KEY_COUNT];

static SDL_Gamepad *pads[SKELE_MAX_PADS];
static uint8_t pad_btn_held[SKELE_MAX_PADS][SKELE_PAD_BUTTON_COUNT];
static uint8_t pad_btn_down[SKELE_MAX_PADS][SKELE_PAD_BUTTON_COUNT];
static float pad_axes[SKELE_MAX_PADS][SKELE_PAD_AXIS_COUNT];

static int32_t mouse_dx = 0;
static int32_t mouse_dy = 0;

static skele_key_t scancode_to_key(SDL_Scancode sc)
{
	switch (sc) {
	case SDL_SCANCODE_A:
		return SKELE_KEY_A;
	case SDL_SCANCODE_B:
		return SKELE_KEY_B;
	case SDL_SCANCODE_C:
		return SKELE_KEY_C;
	case SDL_SCANCODE_D:
		return SKELE_KEY_D;
	case SDL_SCANCODE_E:
		return SKELE_KEY_E;
	case SDL_SCANCODE_F:
		return SKELE_KEY_F;
	case SDL_SCANCODE_G:
		return SKELE_KEY_G;
	case SDL_SCANCODE_H:
		return SKELE_KEY_H;
	case SDL_SCANCODE_I:
		return SKELE_KEY_I;
	case SDL_SCANCODE_J:
		return SKELE_KEY_J;
	case SDL_SCANCODE_K:
		return SKELE_KEY_K;
	case SDL_SCANCODE_L:
		return SKELE_KEY_L;
	case SDL_SCANCODE_M:
		return SKELE_KEY_M;
	case SDL_SCANCODE_N:
		return SKELE_KEY_N;
	case SDL_SCANCODE_O:
		return SKELE_KEY_O;
	case SDL_SCANCODE_P:
		return SKELE_KEY_P;
	case SDL_SCANCODE_Q:
		return SKELE_KEY_Q;
	case SDL_SCANCODE_R:
		return SKELE_KEY_R;
	case SDL_SCANCODE_S:
		return SKELE_KEY_S;
	case SDL_SCANCODE_T:
		return SKELE_KEY_T;
	case SDL_SCANCODE_U:
		return SKELE_KEY_U;
	case SDL_SCANCODE_V:
		return SKELE_KEY_V;
	case SDL_SCANCODE_W:
		return SKELE_KEY_W;
	case SDL_SCANCODE_X:
		return SKELE_KEY_X;
	case SDL_SCANCODE_Y:
		return SKELE_KEY_Y;
	case SDL_SCANCODE_Z:
		return SKELE_KEY_Z;
	case SDL_SCANCODE_0:
		return SKELE_KEY_0;
	case SDL_SCANCODE_1:
		return SKELE_KEY_1;
	case SDL_SCANCODE_2:
		return SKELE_KEY_2;
	case SDL_SCANCODE_3:
		return SKELE_KEY_3;
	case SDL_SCANCODE_4:
		return SKELE_KEY_4;
	case SDL_SCANCODE_5:
		return SKELE_KEY_5;
	case SDL_SCANCODE_6:
		return SKELE_KEY_6;
	case SDL_SCANCODE_7:
		return SKELE_KEY_7;
	case SDL_SCANCODE_8:
		return SKELE_KEY_8;
	case SDL_SCANCODE_9:
		return SKELE_KEY_9;
	case SDL_SCANCODE_F1:
		return SKELE_KEY_F1;
	case SDL_SCANCODE_F2:
		return SKELE_KEY_F2;
	case SDL_SCANCODE_F3:
		return SKELE_KEY_F3;
	case SDL_SCANCODE_F4:
		return SKELE_KEY_F4;
	case SDL_SCANCODE_F5:
		return SKELE_KEY_F5;
	case SDL_SCANCODE_F6:
		return SKELE_KEY_F6;
	case SDL_SCANCODE_F7:
		return SKELE_KEY_F7;
	case SDL_SCANCODE_F8:
		return SKELE_KEY_F8;
	case SDL_SCANCODE_F9:
		return SKELE_KEY_F9;
	case SDL_SCANCODE_F10:
		return SKELE_KEY_F10;
	case SDL_SCANCODE_F11:
		return SKELE_KEY_F11;
	case SDL_SCANCODE_F12:
		return SKELE_KEY_F12;
	case SDL_SCANCODE_SPACE:
		return SKELE_KEY_SPACE;
	case SDL_SCANCODE_RETURN:
		return SKELE_KEY_ENTER;
	case SDL_SCANCODE_ESCAPE:
		return SKELE_KEY_ESCAPE;
	case SDL_SCANCODE_TAB:
		return SKELE_KEY_TAB;
	case SDL_SCANCODE_BACKSPACE:
		return SKELE_KEY_BACKSPACE;
	case SDL_SCANCODE_LSHIFT:
		return SKELE_KEY_LSHIFT;
	case SDL_SCANCODE_RSHIFT:
		return SKELE_KEY_RSHIFT;
	case SDL_SCANCODE_LCTRL:
		return SKELE_KEY_LCTRL;
	case SDL_SCANCODE_RCTRL:
		return SKELE_KEY_RCTRL;
	case SDL_SCANCODE_LALT:
		return SKELE_KEY_LALT;
	case SDL_SCANCODE_RALT:
		return SKELE_KEY_RALT;
	case SDL_SCANCODE_UP:
		return SKELE_KEY_UP;
	case SDL_SCANCODE_DOWN:
		return SKELE_KEY_DOWN;
	case SDL_SCANCODE_LEFT:
		return SKELE_KEY_LEFT;
	case SDL_SCANCODE_RIGHT:
		return SKELE_KEY_RIGHT;
	default:
		return SKELE_KEY_UNKNOWN;
	}
}

static skele_pad_button_t sdl_btn_to_pad(SDL_GamepadButton btn)
{
	switch (btn) {
	case SDL_GAMEPAD_BUTTON_SOUTH:
		return SKELE_PAD_A;
	case SDL_GAMEPAD_BUTTON_EAST:
		return SKELE_PAD_B;
	case SDL_GAMEPAD_BUTTON_WEST:
		return SKELE_PAD_X;
	case SDL_GAMEPAD_BUTTON_NORTH:
		return SKELE_PAD_Y;
	case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:
		return SKELE_PAD_LB;
	case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:
		return SKELE_PAD_RB;
	case SDL_GAMEPAD_BUTTON_LEFT_STICK:
		return SKELE_PAD_LSTICK;
	case SDL_GAMEPAD_BUTTON_RIGHT_STICK:
		return SKELE_PAD_RSTICK;
	case SDL_GAMEPAD_BUTTON_START:
		return SKELE_PAD_START;
	case SDL_GAMEPAD_BUTTON_BACK:
		return SKELE_PAD_BACK;
	case SDL_GAMEPAD_BUTTON_DPAD_UP:
		return SKELE_PAD_DPAD_UP;
	case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
		return SKELE_PAD_DPAD_DOWN;
	case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
		return SKELE_PAD_DPAD_LEFT;
	case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
		return SKELE_PAD_DPAD_RIGHT;
	default:
		return SKELE_PAD_BUTTON_COUNT;
	}
}

static int8_t pad_slot(SDL_JoystickID id)
{
	uint8_t i;
	for (i = 0; i < SKELE_MAX_PADS; i++)
		if (pads[i] && SDL_GetGamepadID(pads[i]) == id)
			return (int8_t)i;
	return -1;
}

static int8_t free_slot(void)
{
	uint8_t i;
	for (i = 0; i < SKELE_MAX_PADS; i++)
		if (!pads[i])
			return (int8_t)i;
	return -1;
}

static void update_axes(uint8_t slot)
{
	pad_axes[slot][SKELE_PAD_AXIS_LX] =
	    SDL_GetGamepadAxis(pads[slot], SDL_GAMEPAD_AXIS_LEFTX) / 32767.0f;
	pad_axes[slot][SKELE_PAD_AXIS_LY] =
	    SDL_GetGamepadAxis(pads[slot], SDL_GAMEPAD_AXIS_LEFTY) / 32767.0f;
	pad_axes[slot][SKELE_PAD_AXIS_RX] =
	    SDL_GetGamepadAxis(pads[slot], SDL_GAMEPAD_AXIS_RIGHTX) / 32767.0f;
	pad_axes[slot][SKELE_PAD_AXIS_RY] =
	    SDL_GetGamepadAxis(pads[slot], SDL_GAMEPAD_AXIS_RIGHTY) / 32767.0f;
	pad_axes[slot][SKELE_PAD_AXIS_LT] =
	    SDL_GetGamepadAxis(pads[slot], SDL_GAMEPAD_AXIS_LEFT_TRIGGER) /
	    32767.0f;
	pad_axes[slot][SKELE_PAD_AXIS_RT] =
	    SDL_GetGamepadAxis(pads[slot], SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) /
	    32767.0f;
}

uint8_t skele_input_poll(void)
{
	SDL_Event e;
	skele_key_t key;
	int8_t slot;
	uint8_t i;
	skele_pad_button_t btn;

	memset(key_down, 0, sizeof(key_down));
	memset(pad_btn_down, 0, sizeof(pad_btn_down));
	mouse_dx = 0;
	mouse_dy = 0;

	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_EVENT_QUIT:
			return 0;

		case SDL_EVENT_MOUSE_MOTION:
			mouse_dx += (int32_t)e.motion.xrel;
			mouse_dy += (int32_t)e.motion.yrel;
			break;

		case SDL_EVENT_KEY_DOWN:
			if (e.key.repeat)
				break;
			switch (e.key.scancode) {
			case SDL_SCANCODE_F11:
				skele_video_toggle_fullscreen();
				break;
			case SDL_SCANCODE_F4:
				skele_video_cycle_scale();
				break;
			default:
				key = scancode_to_key(e.key.scancode);
				if (key != SKELE_KEY_UNKNOWN) {
					key_down[key] = 1;
					key_held[key] = 1;
				}
				break;
			}
			break;

		case SDL_EVENT_KEY_UP:
			key = scancode_to_key(e.key.scancode);
			if (key != SKELE_KEY_UNKNOWN)
				key_held[key] = 0;
			break;

		case SDL_EVENT_GAMEPAD_ADDED:
			slot = free_slot();
			if (slot >= 0) {
				pads[slot] = SDL_OpenGamepad(e.gdevice.which);
				if (pads[slot])
					memset(pad_btn_held[slot], 0,
					       sizeof(pad_btn_held[slot]));
			}
			break;

		case SDL_EVENT_GAMEPAD_REMOVED:
			slot = pad_slot(e.gdevice.which);
			if (slot >= 0) {
				SDL_CloseGamepad(pads[slot]);
				pads[slot] = NULL;
				memset(pad_btn_held[slot], 0,
				       sizeof(pad_btn_held[slot]));
				memset(pad_axes[slot], 0,
				       sizeof(pad_axes[slot]));
			}
			break;

		case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
			slot = pad_slot(e.gbutton.which);
			btn =
			    sdl_btn_to_pad((SDL_GamepadButton)e.gbutton.button);
			if (slot >= 0 && btn != SKELE_PAD_BUTTON_COUNT) {
				pad_btn_down[slot][btn] = 1;
				pad_btn_held[slot][btn] = 1;
			}
			break;

		case SDL_EVENT_GAMEPAD_BUTTON_UP:
			slot = pad_slot(e.gbutton.which);
			btn =
			    sdl_btn_to_pad((SDL_GamepadButton)e.gbutton.button);
			if (slot >= 0 && btn != SKELE_PAD_BUTTON_COUNT)
				pad_btn_held[slot][btn] = 0;
			break;

		default:
			break;
		}
	}

	for (i = 0; i < SKELE_MAX_PADS; i++)
		if (pads[i])
			update_axes(i);

	return 1;
}

uint8_t skele_key_down(skele_key_t key)
{
	return key < SKELE_KEY_COUNT ? key_down[key] : 0;
}

uint8_t skele_key_held(skele_key_t key)
{
	return key < SKELE_KEY_COUNT ? key_held[key] : 0;
}

uint8_t skele_pad_connected(uint8_t pad)
{
	return pad < SKELE_MAX_PADS ? pads[pad] != NULL : 0;
}

uint8_t skele_pad_button_down(uint8_t pad, skele_pad_button_t btn)
{
	return (pad < SKELE_MAX_PADS && btn < SKELE_PAD_BUTTON_COUNT)
		   ? pad_btn_down[pad][btn]
		   : 0;
}

uint8_t skele_pad_button_held(uint8_t pad, skele_pad_button_t btn)
{
	return (pad < SKELE_MAX_PADS && btn < SKELE_PAD_BUTTON_COUNT)
		   ? pad_btn_held[pad][btn]
		   : 0;
}

float skele_pad_axis(uint8_t pad, skele_pad_axis_t axis)
{
	return (pad < SKELE_MAX_PADS && axis < SKELE_PAD_AXIS_COUNT)
		   ? pad_axes[pad][axis]
		   : 0.0f;
}

void skele_mouse_delta(int32_t *dx, int32_t *dy)
{
	if (dx)
		*dx = mouse_dx;
	if (dy)
		*dy = mouse_dy;
}
