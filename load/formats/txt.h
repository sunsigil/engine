#ifndef TXT_H
#define TXT_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct TXT
{
	size_t file_size;
	char* text;
} TXT_t;

void TXT_init(TXT_t* txt, const char* path);
void TXT_dispose(TXT_t* txt);

#ifdef __cplusplus
}
#endif

#endif
