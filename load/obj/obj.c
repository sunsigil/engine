#include "obj.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

typedef struct string_cut
{
	const char* start;
	size_t length;
} string_cut_t;


void OBJ_init(OBJ_t* obj, const char* path)
{
	int fd = open(path, O_RDONLY, S_IRUSR);
	if(fd == -1)
	{
		perror("[OBJ_read] open");
		exit(EXIT_FAILURE);
	}
	size_t file_size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	uint8_t* file_content = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, SEEK_SET);
	if(file_content == NULL)
	{
		perror("[OBJ_read] mmap");
		exit(EXIT_FAILURE);
	}
	close(fd);

	// Count lines
	int line_count = 0;
	char* ptr = (char*) file_content;
	while(ptr < (char*) (file_content + file_size))
	{
		if(*ptr == '\n')
		{ line_count++; }
		ptr++;
	}
	
	// Mark lines and count elements
	string_cut_t* lines = malloc(sizeof(string_cut_t) * line_count);
	int v_count = 0;
	int vt_count = 0;
	int vn_count = 0;
	int f_count = 0;
	ptr = (char*) file_content;
	for(int i = 0; i < line_count; i++)
	{
		lines[i].start = ptr;
		ptr = strchr(ptr, '\n')+1;
		lines[i].length = ptr - lines[i].start;

		if(strncmp(lines[i].start, "v ", 2) == 0)
		{ v_count++; }
		else if(strncmp(lines[i].start, "vt ", 3) == 0)
		{ vt_count++; }
		else if(strncmp(lines[i].start, "vn ", 3) == 0)
		{ vn_count++; }
		else if(strncmp(lines[i].start, "f ", 2) == 0)
		{ f_count++; }
	}

	// Parse lines into elements
	size_t vs_size = sizeof(float) * v_count * OBJ_V_STRIDE;
	size_t vts_size = sizeof(float) * vt_count * OBJ_VT_STRIDE;
	size_t vns_size = sizeof(float) * vn_count * OBJ_VN_STRIDE;
	size_t fs_size = sizeof(float) * f_count * OBJ_F_STRIDE;
	float* vs = malloc(vs_size);
	float* vts = malloc(vts_size);
	float* vns = malloc(vns_size);
	int* fs = malloc(fs_size);
	int v_idx = 0;
	int vt_idx = 0;
	int vn_idx = 0;
	int f_idx = 0;
	for(int i = 0; i < line_count; i++)
	{
		char* ptr = (char*) lines[i].start;
		if(strncmp(ptr, "v ", 2) == 0)
		{
			for(int j = 0; j < OBJ_V_STRIDE; j++)
			{
				ptr = strchr(ptr, ' ')+1;
				vs[v_idx*OBJ_V_STRIDE+j] = atof(ptr);
			}
			v_idx++;
		}
		else if(strncmp(ptr, "vt ", 3) == 0)
		{
			for(int j = 0; j < OBJ_VT_STRIDE; j++)
			{
				ptr = strchr(ptr, ' ')+1;
				vts[vt_idx*OBJ_VT_STRIDE+j] = atof(ptr);
			}
			vt_idx++;
		}
		else if(strncmp(ptr, "vn ", 3) == 0)
		{
			for(int j = 0; j < OBJ_VN_STRIDE; j++)
			{
				ptr = strchr(ptr, ' ')+1;
				vns[vn_idx*OBJ_VN_STRIDE+j] = atof(ptr);
			}
			vn_idx++;
		}
		else if(strncmp(ptr, "f ", 2) == 0)
		{
			for(int j = 0; j < OBJ_F_POINT_COUNT; j++)
			{
				ptr = strchr(ptr, ' ')+1;
				if(strnstr(lines[i].start, "//", lines[i].length)) // f v//vn v//vn v//vn
				{
					fs[f_idx*OBJ_F_STRIDE+j*OBJ_F_POINT_STRIDE+0] = atoi(ptr);
					fs[f_idx*OBJ_F_STRIDE+j*OBJ_F_POINT_STRIDE+1] = 0;
					ptr = strstr(ptr, "//")+2;
					fs[f_idx*OBJ_F_STRIDE+j*OBJ_F_POINT_STRIDE+2] = atoi(ptr);
				}
				else
				{
					int slash_count = 0;
					for(char* c = (char*) lines[i].start; c < lines[i].start + lines[i].length; c++)
					{
						if(*c == '/')
						{ slash_count++; }
					}

					if(slash_count == 6) // f v/vt/vn v/vt/vn v/vt/vn
					{
						fs[f_idx*OBJ_F_STRIDE+j*OBJ_F_POINT_STRIDE+0] = atoi(ptr);
						ptr = strchr(ptr, '/')+1;
						fs[f_idx*OBJ_F_STRIDE+j*OBJ_F_POINT_STRIDE+1] = atoi(ptr);
						ptr = strchr(ptr, '/')+1;
						fs[f_idx*OBJ_F_STRIDE+j*OBJ_F_POINT_STRIDE+2] = atoi(ptr);
					}
					else if(slash_count == 3) // f v/vt v/vt v/vt
					{
						fs[f_idx*OBJ_F_STRIDE+j*OBJ_F_POINT_STRIDE+0] = atoi(ptr);
						ptr = strchr(ptr, '/')+1;
						fs[f_idx*OBJ_F_STRIDE+j*OBJ_F_POINT_STRIDE+1] = atoi(ptr);
						fs[f_idx*OBJ_F_STRIDE+j*OBJ_F_POINT_STRIDE+2] = 0;
					}
					else if(slash_count == 0) // f v v v
					{
						fs[f_idx*OBJ_F_STRIDE+j*OBJ_F_POINT_STRIDE+0] = atoi(ptr);
						fs[f_idx*OBJ_F_STRIDE+j*OBJ_F_POINT_STRIDE+1] = 0;
						fs[f_idx*OBJ_F_STRIDE+j*OBJ_F_POINT_STRIDE+2] = 0;
					}
				}
			}
			f_idx++;
		}
	}
	
	free(lines);
	munmap(file_content, file_size);

	obj->v_count = v_count;
	obj->vt_count = vt_count;
	obj->vn_count = vn_count;
	obj->f_count = f_count;
	obj->vs_size = vs_size;
	obj->vts_size = vts_size;
	obj->vns_size = vns_size;
	obj->fs_size = fs_size;
	obj->vs = vs;
	obj->vts = vts;
	obj->vns = vns;
	obj->fs = fs;
}

void OBJ_dispose(OBJ_t* obj)
{
	free(obj->vs);
	free(obj->vts);
	free(obj->vns);
	free(obj->fs);
}
