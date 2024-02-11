#ifndef OBJ_H
#define OBJ_H

#include <stdlib.h>

#define OBJ_V_STRIDE 3
#define OBJ_VT_STRIDE 2
#define OBJ_VN_STRIDE 3
#define OBJ_F_POINT_STRIDE 3
#define OBJ_F_POINT_COUNT 3
#define OBJ_F_STRIDE 9

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct OBJ
{
	int v_count;
	int vt_count;
	int vn_count;
	int f_count;

	size_t vs_size;
	size_t vts_size;
	size_t vns_size;
	size_t fs_size;

	float* vs;
	float* vts;
	float* vns;
	int* fs;
} OBJ_t;

void OBJ_init(OBJ_t* obj, const char* path);
void OBJ_dispose(OBJ_t* obj);

#ifdef __cplusplus
}
#endif

#endif
