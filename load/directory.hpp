#pragma once

#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <iostream>

enum dir_node_type_t
{
	DIR_INTERNAL,
	DIR_LEAF
};

struct dir_node_t
{
	dir_node_type_t type;

	std::string path;
	std::string stem;
	std::string name;
	std::string extension;

	std::vector<dir_node_t*> children;
};

void dir_node_init(dir_node_t* node, std::string path)
{
	struct stat st;
	if(stat(path.c_str(), &st) != 0)
	{
		perror("stat");
		exit(EXIT_FAILURE);
	}
	
	node->path = path;
	size_t last_slash = path.rfind('/');
	size_t last_dot = path.rfind('.');
	size_t last_char = path.size()-1;
	node->stem = path.substr(0, last_slash);
	node->name = path.substr(last_slash+1, last_dot-last_slash-1);
	if(last_dot != -1)
	{
		node->extension = path.substr(last_dot+1, last_char-last_dot);
	}
	else
	{
		node->extension = "";
	}
	
	node->children = std::vector<dir_node_t*>();
	if(S_ISDIR(st.st_mode))
	{
		node->type = DIR_INTERNAL;

		DIR* dir = opendir(path.c_str());
		if(dir == NULL)
		{
			perror("opendir");
			exit(EXIT_FAILURE);
		}
		struct dirent* ent;

		while((ent = readdir(dir)) != NULL)
		{
			if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
			{
				continue;
			}

			char child_path_cstr[PATH_MAX];
			snprintf(child_path_cstr, PATH_MAX, "%s/%s", path.c_str(), ent->d_name);
			std::string child_path = std::string(child_path_cstr);

			dir_node_t* child = new dir_node_t;
			dir_node_init(child, child_path);
			node->children.push_back(child);
		}

		closedir(dir);
	}
	else
	{
		node->type = DIR_LEAF;
	}
}

void dir_node_dispose(dir_node_t* node)
{
	for(int i = 0; i < node->children.size(); i++)
	{
		dir_node_dispose(node->children[i]);
		free(node->children[i]);
	}
}

void dir_pick_leaves(dir_node_t* node, std::vector<dir_node_t*>& leaves)
{
	if(node->type == DIR_LEAF)
	{
		leaves.push_back(node);
	}
	else
	{
		for(int i = 0; i < node->children.size(); i++)
		{
			dir_pick_leaves(node->children[i], leaves);
		}
	}
}
