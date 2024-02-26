#ifndef MTL_H
#define MTL_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct MTL
{
	char* name;
	float* Ka;
	float* Kd;
	float* Ks;
	float d;
	int halo;
	int Ns;
	char* map_Ka;
	char* map_Kd;
	char* map_Ks;
	char* map_d;
} MTL_t;

void MTL_init(MTL_t* obj, const char* path);
void MTL_dispose(MTL_t* mtl);

#ifdef __cplusplus
}
#endif

#endif
