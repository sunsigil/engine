#include <ctime>

#include "window.hpp"
#include "gui.hpp"

int main(int argc, char** argv)
{
	window_t window;
	window_init(window, "Engine", glm::vec2(1280, 720));

	gui_state_t gui_state;
	gui_state.width = window.dimensions.x;
	gui_state.height = window.dimensions.y;
	gui_state.flags = window.imgui_flags;
	gui_state.renderer_name = window.renderer_str;
	gui_state.renderer_version = window.version_str;
	
	float last_time = glfwGetTime();
	float time;
	float dt;

	while(!glfwWindowShouldClose(window.handle))
	{
		time = glfwGetTime();
		dt = time - last_time;
		last_time = time;

		glfwPollEvents();
		
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		gui_state.fps = 1.0f/dt;
		gui_begin(gui_state);
		gui_draw_info(gui_state);
		gui_end();
		
		glfwSwapBuffers(window.handle);
	}

	window_dispose(window);
	return 0;
}
