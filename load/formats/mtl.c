#include "mtl.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <math.h>

int MTL_match_token(char* read_head, const char* token)
{
	return memcmp(read_head, token, strlen(token)) == 0;
}

void MTL_next_line(char** read_head)
{
	while(**read_head != '\n')
	{
		if(**read_head == '\0')
		{
			fprintf(stderr, "!\n");
			*read_head -= 1;
			return;
		}
		*read_head += 1;
	}
}

void MTL_init(MTL_t* mtl, const char* path)
{
	int fd = open(path, O_RDONLY, S_IRUSR);
	if(fd == -1)
	{
		perror("[MTL_init] open");
		exit(EXIT_FAILURE);
	}
	size_t file_size = lseek(fd, 0, SEEK_END);
	char* file_content = (char*) malloc(file_size+1);
	lseek(fd, 0, SEEK_SET);
	size_t bytes_read = read(fd, file_content, file_size);
	if(bytes_read < file_size)
	{
		perror("[MTL_init] read");
		exit(EXIT_FAILURE);
	}
	file_content[file_size] = '\0';
	close(fd);

	// Initialize elements
	mtl->name = NULL;
	mtl->Ka = NULL;
	mtl->Kd = NULL;
	mtl->Ks = NULL;
	mtl->d = 1;
	mtl->halo = 0;
	mtl->Ns = 0;
	mtl->map_Ka = NULL;
	mtl->map_Kd = NULL;
	mtl->map_Ks = NULL;
	mtl->map_d = NULL;

	// Parse lines into elements
	char* read_head = file_content;
	while(*read_head != '\0')
	{
		if(MTL_match_token(read_head, "newmtl"))
		{
			mtl->name = malloc(512);
			sscanf(read_head, "newmtl %s", mtl->name);	
			MTL_next_line(&read_head);
		}
		else if(MTL_match_token(read_head, "Ka"))
		{
			mtl->Ka = malloc(sizeof(float) * 3);
			sscanf(read_head, "Ka %f %f %f", mtl->Ka+0, mtl->Ka+1, mtl->Ka+2);
			MTL_next_line(&read_head);
		}
		else if(MTL_match_token(read_head, "Kd"))
		{
			mtl->Kd = malloc(sizeof(float) * 3);
			sscanf(read_head, "Kd %f %f %f", mtl->Kd+0, mtl->Kd+1, mtl->Kd+2);
			MTL_next_line(&read_head);
		}
		else if(MTL_match_token(read_head, "Ks"))
		{
			mtl->Ks = malloc(sizeof(float) * 3);
			sscanf(read_head, "Ks %f %f %f", mtl->Ks+0, mtl->Ks+1, mtl->Ks+2);
			MTL_next_line(&read_head);
		}
		else if(MTL_match_token(read_head, "d"))
		{
			sscanf(read_head, "d %f", &mtl->d);
		}
		else if(MTL_match_token(read_head, "Ns"))
		{
			sscanf(read_head, "Ns %d", &mtl->Ns);
		}
		else if(MTL_match_token(read_head, "map_Ka"))
		{
			mtl->map_Ka = malloc(512);
			sscanf(read_head, "map_Ka %s", mtl->map_Ka);	
			MTL_next_line(&read_head);
		}
		else if(MTL_match_token(read_head, "map_Kd"))
		{
			mtl->map_Kd = malloc(512);
			sscanf(read_head, "map_Kd %s", mtl->map_Kd);	
			MTL_next_line(&read_head);
		}
		else if(MTL_match_token(read_head, "map_Ks"))
		{
			mtl->map_Ks = malloc(512);
			sscanf(read_head, "map_Ks %s", mtl->map_Ks);	
			MTL_next_line(&read_head);
		}
		else if(MTL_match_token(read_head, "map_d"))
		{
			mtl->map_d = malloc(512);
			sscanf(read_head, "map_d %s", mtl->map_d);	
			MTL_next_line(&read_head);
		}
		read_head += 1;
	}
}

void MTL_free(void* ptr)
{
	if(ptr != NULL)
		free(ptr);
}

void MTL_dispose(MTL_t* mtl)
{
	MTL_free(mtl->name);
	MTL_free(mtl->Ka);
	MTL_free(mtl->Kd);
	MTL_free(mtl->Ks);
	MTL_free(mtl->map_Ka);
	MTL_free(mtl->map_Kd);
	MTL_free(mtl->map_Ks);
	MTL_free(mtl->map_d);
}
