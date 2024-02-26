#ifndef BMPM_H
#define BMPM_H

#include "bmp.h"

#ifdef __cplusplus
extern "C"
{
#endif

BMP_t BMPM_map(const char* path);
colour_t BMPM_get_pixel(BMP_t* bmp, int x, int y);
int BMPM_unmap(BMP_t* bmp);

#ifdef __cplusplus
}
#endif

#endif
