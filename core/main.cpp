#include "window.hpp"

int main(int argc, char** argv)
{
	window_t window;
	window_init(window, "Engine", glm::vec2(1280, 720));
	
	while(!glfwWindowShouldClose(window.handle))
	{
		glfwPollEvents();
		
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glfwSwapBuffers(window.handle);
	}

	window_dispose(window);
	return 0;
}
