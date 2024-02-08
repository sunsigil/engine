#ifndef TXT_H
#define TXT_H

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

void txt_read(char** text, const char* path)
{
	int fd = open(path, O_RDONLY);
	if(fd == -1)
	{
		perror("[read_text] open");
		exit(EXIT_FAILURE);
	}
	size_t file_size = lseek(fd, 0, SEEK_END);
	*text = (char*) malloc(file_size+1);
	lseek(fd, 0, SEEK_SET);
	size_t bytes_read = read(fd, *text, file_size);
	*text[file_size] = '\0';
	if(bytes_read < file_size)
	{
		perror("[read_text_file] read");
		exit(EXIT_FAILURE);
	}
	close(fd);
}

#endif
