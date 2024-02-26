#include "mtl.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <math.h>

typedef enum MTL_consume_mode
{
	GLYPHS,
	SPACES,
	LINE
} MTL_consume_mode_t;

size_t MTL_consume(char** read_head, MTL_consume_mode_t mode)
{
	size_t consumed = 0;
	while(**read_head != '\0')
	{
		if
		(
		 	(mode == GLYPHS && isspace(**read_head)) |
			(mode == SPACES && !isspace(**read_head))
		)
		{
			break;
		}
		else if(mode == LINE && **read_head == '\n')
		{
			*read_head += 1;
			break;
		}

		consumed += 1;
		*read_head += 1;
	}

	if(consumed == 0)
	{
		fprintf(stderr, "[MTL_consume] error: consume failed in mode %d\n", mode);
		exit(EXIT_FAILURE);
	}

	return consumed;
}

char* MTL_parse_string(char** read_head)
{
	if(isspace(**read_head))
	{			
		fprintf(stderr, "[MTL_parse_string] error: string must not begin with space\n");
		exit(EXIT_FAILURE);
	}

	char* start = *read_head;
	size_t length = 0;
	while(**read_head != '\0')
	{
		if(isspace(**read_head))
		{
			char* string = malloc(length+1);
			memcpy(&string, start, length);
			string[length] = '\0';
			return string;
		}
		else
		{
			*read_head += 1;
			length += 1;
		}
	}
	
	fprintf(stderr, "[MTL_parse_string] error: unexpected EOF\n");
	exit(EXIT_FAILURE);
}

float MTL_parse_num(char** read_head)
{
	if(!isdigit(**read_head) && **read_head != '-' && **read_head != '.')
	{
		fprintf(stderr, "[MTL_parse_num] error: unexpected character '%c'. number must begin with a digit, '-', or '.'\n", **read_head);
		exit(EXIT_FAILURE);
	}

	int whole_body = 0;

	int after_point = 0;
	float frac_place = 0.1f;
	float frac_body = 0;

	int after_exp = 0;
	int exp_body = 0;
	
	int sign = 1;

	while(**read_head != '\0')
	{
		if(**read_head == '-')
		{
			sign = -1;
		}
		else if(**read_head == '.')
		{
			after_point = 1;
		}
		else if(**read_head == 'e' || **read_head == 'E')
		{
			after_exp = 1;
			sign = 1;
		}
		else if(isdigit(**read_head))
		{
			int digit = **read_head - '0';
			if(after_exp)
			{
				exp_body *= 10;
				exp_body += digit * sign;
			}
			else if(after_point)
			{
				frac_body += digit * sign * frac_place;
				frac_place /= 10.0f;
			}
			else
			{
				whole_body *= 10;
				whole_body += digit * sign;
			}
		}
		else
		{
			float num = (whole_body + frac_body) * pow(10, exp_body);
			*read_head -= 1;
			return num;
		}
		
		*read_head += 1;
	}

	fprintf(stderr, "[MTL_parse_num] error: unexpected EOF\n");
	exit(EXIT_FAILURE);
}

float* MTL_parse_vec(char** read_head)
{
	float* vec = malloc(sizeof(float) * 3);
	vec[0] = MTL_parse_num(read_head);
	MTL_consume(read_head, SPACES);
	vec[1] = MTL_parse_num(read_head);
	MTL_consume(read_head, SPACES);
	vec[2] = MTL_parse_num(read_head);
	return vec;
}

int MTL_assert(char** read_head, const char* token)
{
	if(memcmp(*read_head, token, strlen(token)) == 0)
	{
		fprintf(stderr, "%s\n", token);
		*read_head += strlen(token);
		MTL_consume(read_head, SPACES);
		return 1;
	}
	return 0;
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
		if(MTL_assert(&read_head, "newmtl"))
		{
			mtl->name = MTL_parse_string(&read_head);
		}
		else if(MTL_assert(&read_head, "Ka"))
		{
			mtl->Ka = MTL_parse_vec(&read_head);
		}
		else if(MTL_assert(&read_head, "Kd"))
		{
			mtl->Kd = MTL_parse_vec(&read_head);
		}
		else if(MTL_assert(&read_head, "Ks"))
		{
			mtl->Ks = MTL_parse_vec(&read_head);
		}
		else if(MTL_assert(&read_head, "d"))
		{
			mtl->d = MTL_parse_num(&read_head);
		}
		else if(MTL_assert(&read_head, "Ns"))
		{
			mtl->Ns = MTL_parse_num(&read_head);
		}
		else if(MTL_assert(&read_head, "map_Ka"))
		{
			mtl->map_Ka = MTL_parse_string(&read_head);
		}
		else if(MTL_assert(&read_head, "map_Kd"))
		{
			mtl->map_Kd = MTL_parse_string(&read_head);
		}
		else if(MTL_assert(&read_head, "map_Ks"))
		{
			mtl->map_Ks = MTL_parse_string(&read_head);
		}
		else if(MTL_assert(&read_head, "map_d"))
		{
			mtl->map_d = MTL_parse_string(&read_head);
		}
		*read_head += 1;
	}
}

void MTL_dispose(MTL_t* mtl)
{
	free(mtl->name);
	free(mtl->Ka);
	free(mtl->Kd);
	free(mtl->Ks);
	free(mtl->map_Ka);
	free(mtl->map_Kd);
	free(mtl->map_Ks);
	free(mtl->map_d);
}
