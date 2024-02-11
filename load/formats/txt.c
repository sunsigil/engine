#include "txt.h"

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

void TXT_init(TXT_t* txt, const char* path)
{
	int fd = open(path, O_RDONLY);
	if(fd == -1)
	{
		perror("[TXT_read] open");
		exit(EXIT_FAILURE);
	}
	size_t file_size = lseek(fd, 0, SEEK_END);
	char* text = (char*) malloc(file_size+1);
	lseek(fd, 0, SEEK_SET);
	size_t bytes_read = read(fd, text, file_size);
	if(bytes_read < file_size)
	{
		perror("[TXT_read] read");
		exit(EXIT_FAILURE);
	}
	text[file_size] = '\0';
	close(fd);

	txt->file_size = file_size;
	txt->text = text;
}
