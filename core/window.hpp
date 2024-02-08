#pragma once

#include <iostream>
#include <cstring>

#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "misc/cpp/imgui_stdlib.h"

#include <glm/glm.hpp>

struct window_t
{
	GLFWwindow* handle;

	glm::ivec2 dimensions;
	ImGuiWindowFlags imgui_flags;

	std::string title_str;
	std::string renderer_str;
	std::string version_str;

	glm::vec2 mouse_pos;
};

void window_init(window_t& window, std::string name, glm::ivec2 dimensions)
{
	if(!glfwInit()) 
	{
		std::cerr << "[window_init] failed to initialize GLFW!" << std::endl;
		exit(EXIT_FAILURE);
	} 

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  
	GLFWwindow* handle = glfwCreateWindow(dimensions.x, dimensions.y, name.c_str(), NULL, NULL);
	if(!handle) 
	{
		std::cerr << "[window_init] failed to open GLFW window!" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
  	glfwMakeContextCurrent(handle);
								  
  	glewExperimental = GL_TRUE;
  	if(glewInit() != GLEW_OK)
	{
		std:: cerr << "[window_init] failed to initialize GLEW!" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	const GLubyte* renderer_glstr = glGetString(GL_RENDERER);
	const GLubyte* version_glstr = glGetString(GL_VERSION);
	char renderer_cstr[512];
	char version_cstr[512];
	strcpy(renderer_cstr, (const char*) renderer_glstr);
	strcpy(version_cstr, (const char*) version_glstr);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

	ImGuiWindowFlags imgui_flags =
	ImGuiWindowFlags_NoSavedSettings |
	ImGuiWindowFlags_NoMove |
	ImGuiWindowFlags_NoResize |
	ImGuiWindowFlags_NoSavedSettings |
	ImGuiWindowFlags_NoCollapse |
	ImGuiWindowFlags_NoTitleBar |
	ImGuiWindowFlags_NoBackground;
	
	ImGui_ImplGlfw_InitForOpenGL(handle, true);
	ImGui_ImplOpenGL3_Init("#version 150");

	window.handle = handle;
	window.dimensions = dimensions;
	window.imgui_flags = imgui_flags;
	window.title_str = name;
  	window.renderer_str = std::string(renderer_cstr);
  	window.version_str = std::string(version_cstr);
}

void window_dispose(window_t& window)
{
	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

	glfwDestroyWindow(window.handle);
  	glfwTerminate();
}

glm::vec2 get_mouse_pos(window_t& window)
{
	double x, y;
	glfwGetCursorPos(window.handle, &x, &y);
	x /= window.dimensions.x; y /= window.dimensions.y;
	x -= 0.5f; y -= 0.5f;
	x *= 2.0f; y *= 2.0f; 
	return glm::vec2(x, y);
}

