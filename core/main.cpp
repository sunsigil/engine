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
	asset_bank_t assets;
	asset_bank_init(assets, asset_dir);
	scene_bank_t scene_bank;
	scene_bank_init(scene_bank, asset_dir, assets);
	dir_node_dispose(asset_dir);

	scene_t scene = scene_bank.scenes["playground"];
	
	/*transform_t grid_transform = transform_t();
	mesh_t grid_mesh(gen_AA_plane(Z, CW), 6, GL_TRIANGLES);
	material_t grid_material = material_t();
	renderer_t grid_renderer(&grid_mesh, assets.shaders["grid"], &grid_material);
	scene_add(scene, "grid", grid_transform, grid_renderer);*/

	transform_t cam_transform = transform_t();
	camera_t camera = camera_t(cam_transform, 1, window.window_size.x / (float) window.window_size.y, 0.1f, 1000.0f);

	gui_state_t gui_state;
	gui_state.window = &window;
	gui_state.scene = &scene;
	
	float last_time = glfwGetTime();
	float time;
	float dt;

	glm::vec4 lights[2] = 
	{
		glm::vec4(0, 100, 0, 0),
		glm::vec4(0, 0, 0, 1),
	};

	GLuint shadow_fbo;
	glGenFramebuffers(1, &shadow_fbo);
	GLuint shadow_map;
	glGenTextures(1, &shadow_map);
	
	glBindTexture(GL_TEXTURE_2D, shadow_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
	
	glViewport(0, 0, 1024, 1024);
	glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_map, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glViewport(0, 0, window.frame_size.x, window.frame_size.y);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glViewport(0, 0, 1024, 1024);
	glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);
	float shadow_near = 1.0f, shadow_far = 100.0f;
	glm::mat4 V_light = glm::lookAt(glm::vec3(lights[0]), glm::vec3(0), glm::vec3(0,1,0));
	glm::mat4 P_light = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, shadow_near, shadow_far);
	glm::mat4 L = P_light * V_light;	
	shader_t* shadow_shader = assets.shaders["dirshade"];
    glClear(GL_DEPTH_BUFFER_BIT);
	for(int i = 0; i < scene.size; i++)
	{
		transform_t transform = scene.transforms[i];
		renderer_t renderer = scene.renderers[i];
		glUseProgram(shadow_shader->prog_id);
		glBindVertexArray(renderer.mesh->vao_id);
		set_mat4_uniform(shadow_shader, "M", transform.make_model());
		set_mat4_uniform(shadow_shader, "L", L);
		glDrawArrays(renderer.mesh->mode, 0, renderer.mesh->vert_count);
		glBindVertexArray(0);
		glUseProgram(0);
	}
	glViewport(0, 0, window.frame_size.x, window.frame_size.y);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
		
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for(int i = 0; i < scene.size; i++)
		{
			transform_t transform = scene.transforms[i];
			renderer_t renderer = scene.renderers[i];
			render(camera, renderer, transform, lights, L, shadow_map);
		}
		
		mesh_t screen_mesh(gen_AA_plane(Z, CW), 6, GL_TRIANGLES);
		shader_t* screen_shader = assets.shaders["screentex"];
		glUseProgram(screen_shader->prog_id);
		glBindVertexArray(screen_mesh.vao_id);
		set_tex2_uniform(screen_shader, "screen_tex", 0, shadow_map);
		glDrawArrays(screen_mesh.mode, 0, screen_mesh.vert_count);
		glBindVertexArray(0);
		glUseProgram(0);

		gui_begin(gui_state);
		gui_draw_window_info(gui_state, 1.0f/dt);
		gui_draw_scene_info(gui_state);
		gui_end();
		
		glfwSwapBuffers(window.handle);
	}

	window_dispose(window);
	return 0;
}
