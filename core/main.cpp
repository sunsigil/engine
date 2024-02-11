#include <ctime>

#include "window.hpp"
#include "gui.hpp"
#include "bank.hpp"
#include "transform.hpp"
#include "camera.hpp"
#include "renderer.hpp"

int main(int argc, char** argv)
{
	window_t window;
	window_init(window, "Engine", glm::vec2(1280, 720));

	bank_t bank;
	bank_init(&bank, "assets");

	camera_t camera;
	camera_init(camera, 1, window.dimensions.x / (float) window.dimensions.y, 0.1f, 1000.0f);
	transform_t camera_transform;
	transform_init(camera_transform, glm::vec3(1), glm::quat(glm::vec3(0)), glm::vec3(0));

	renderer_t renderer;
	renderer.mesh = bank.meshes["cube"];
	renderer.shader = bank.shaders["standard"];
	renderer.texture = bank.textures["uv"];
	transform_t renderer_transform;
	transform_init(renderer_transform, glm::vec3(1), glm::quat(glm::vec3(0)), glm::vec3(0,0,2));

	gui_state_t gui_state;
	gui_state.window = &window;
	
	float last_time = glfwGetTime();
	float time;
	float dt;
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while(!glfwWindowShouldClose(window.handle))
	{
		time = glfwGetTime();
		dt = time - last_time;
		last_time = time;

		glfwPollEvents();
		glm::vec2 mouse = get_mouse_pos(window);
		camera_transform.orientation = glm::quat(glm::vec3(mouse.y, -mouse.x, 0));
		glm::vec3 forward = make_forward(camera_transform);
		glm::vec3 right = make_right(camera_transform);
		glm::vec3 wasd = glm::vec3(0,0,0);
		if(glfwGetKey(window.handle, GLFW_KEY_W) == GLFW_PRESS)
		{ wasd += forward; }
		if(glfwGetKey(window.handle, GLFW_KEY_A) == GLFW_PRESS)
		{ wasd += right; }
		if(glfwGetKey(window.handle, GLFW_KEY_S) == GLFW_PRESS)
		{ wasd -= forward; }
		if(glfwGetKey(window.handle, GLFW_KEY_D) == GLFW_PRESS)
		{ wasd -= right; }
		wasd = glm::length(wasd) > 0.0f ? glm::normalize(wasd) : wasd;
		camera_transform.velocity = wasd * (glfwGetKey(window.handle, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? 10.0f : 5.0f);
		euler_integrate(camera_transform, dt);
		
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render(renderer, make_model(renderer_transform), make_view(camera, camera_transform.position, camera_transform.orientation), camera.proj);

		gui_begin(gui_state);
		gui_draw_window_info(gui_state, 1.0f/dt);
		gui_draw_transform(gui_state, renderer_transform);
		gui_end();
		
		glfwSwapBuffers(window.handle);
	}

	window_dispose(window);
	return 0;
}
