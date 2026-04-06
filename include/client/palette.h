#ifndef SKELE_PALETTE_H
#define SKELE_PALETTE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SKELE_PALETTE_COLORS 256

typedef uint32_t skele_palette_t[SKELE_PALETTE_COLORS];

void skele_palette_set(skele_palette_t pal);
void skele_palette_set_index(uint8_t index, uint32_t color);

#ifdef __cplusplus
}
#endif

#endif /* SKELE_PALETTE_H */
