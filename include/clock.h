#ifndef SKELE_CLOCK_H
#define SKELE_CLOCK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void     skele_clock_init(void (*on_signal)(void));
uint64_t skele_time_ns(void);
void     skele_sleep_ns(uint64_t ns);

#ifdef __cplusplus
}
#endif

#endif /* SKELE_CLOCK_H */
