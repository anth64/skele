#ifndef SKELE_H
#define SKELE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SKELE_VERSION_MAJOR 0
#define SKELE_VERSION_MINOR 0
#define SKELE_VERSION_PATCH 0

#define SKELE_STR(x) #x
#define SKELE_XSTR(x) SKELE_STR(x)
#define SKELE_VERSION                                                          \
	SKELE_XSTR(SKELE_VERSION_MAJOR)                                        \
	"." SKELE_XSTR(SKELE_VERSION_MINOR) "." SKELE_XSTR(SKELE_VERSION_PATCH)

#define SKELE_INIT_SUCCESS 1
#define SKELE_INIT_FAILURE 0

#define SKELE_DEFAULT_TICK_RATE 35

extern uint64_t skele_tick_ns;

void skele_set_tick_rate(uint8_t rate);
uint8_t skele_init(void);
void skele_shutdown(void);
void skele_tick(void);

uint8_t skele_stk_setup(void);
void skele_stk_teardown(void);

#ifdef __cplusplus
}
#endif

#endif /* SKELE_H */
