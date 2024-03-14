#include <ctime>

#include "window.hpp"
#include "gui.hpp"
#include "transform.hpp"
#include "camera.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "meshprim.hpp"
#include "directory.hpp"
#include "asset_bank.hpp"
#include "scene_bank.hpp"

int main(int argc, char** argv)
{
	window_t window;
	window_init(window, "Engine", glm::vec2(1280, 720));
	window_toggle_cursor(window, false);

	dir_node_t* asset_dir = new dir_node_t;
	dir_node_init(asset_dir, "assets");
	asset_bank_t asset_bank;
	asset_bank_init(asset_bank, asset_dir);
	scene_bank_t scene_bank;
	scene_bank_init(scene_bank, asset_dir, asset_bank);
	dir_node_dispose(asset_dir);

	scene_t scene = scene_bank.scenes["playground"];
	
	transform_t grid_transform = transform_t();
	mesh_t grid_mesh(gen_AA_plane(Z, CW), 6, GL_TRIANGLES);
	material_t grid_material = material_t();
	renderer_t grid_renderer(&grid_mesh, asset_bank.shaders["grid"], &grid_material);
	scene_add(scene, "grid", grid_transform, grid_renderer);

	transform_t cam_transform = transform_t();
	camera_t camera = camera_t(cam_transform, 1, window.dimensions.x / (float) window.dimensions.y, 0.1f, 1000.0f);

	gui_state_t gui_state;
	gui_state.window = &window;
	gui_state.scene = &scene;
	
	float last_time = glfwGetTime();
	float time;
	float dt;

	glm::vec4 lights[2] = 
	{
		glm::vec4(0, 10, 0, 1),
		glm::vec4(0, 0, 0, 1),
	};
	
	configure_canvas();
	while(!glfwWindowShouldClose(window.handle))
	{
		time = glfwGetTime();
		dt = time - last_time;
		last_time = time;

		glfwPollEvents();

		if(glfwGetKey(window.handle, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			window_toggle_cursor(window, !window.cursor_enabled);
		}

		if(!window.cursor_enabled)
		{
			glm::vec2 mouse = get_mouse_pos(window);
			camera.transform.orientation = glm::quat(glm::vec3(mouse.y, -mouse.x, 0));
			glm::vec3 forward = camera.transform.make_forward();

			glm::vec3 right = camera.transform.make_right();
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
			camera.transform.velocity = wasd * (glfwGetKey(window.handle, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? 10.0f : 5.0f);

			euler_integrate(camera.transform, dt);
		}

		lights[1] = glm::vec4(10 * glm::cos(time), 5, 10 * glm::sin(time), 1);
		
		clear_canvas();
		for(int i = 0; i < scene.size; i++)
		{
			transform_t transform = scene.transforms[i];
			renderer_t renderer = scene.renderers[i];
			render(camera, renderer, transform, lights);
		}

		gui_begin(gui_state);
		gui_draw_window_info(gui_state, 1.0f/dt);
		gui_draw_scene_info(gui_state);
		gui_end();
		
		glfwSwapBuffers(window.handle);
	}

	window_dispose(window);
	return 0;
}
