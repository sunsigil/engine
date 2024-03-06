#pragma once

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

#include <glm/gtc/type_ptr.hpp>

#include "window.hpp"
#include "transform.hpp"
#include "scene.hpp"
#include "asset_bank.hpp"

struct gui_state_t
{
	window_t* window;
	scene_t* scene;
	asset_bank_t* asset_bank;
};

void gui_begin(gui_state_t& state)
{
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(state.window->dimensions.x, state.window->dimensions.y));
	ImGui::Begin("##window", nullptr, state.window->imgui_flags);
}

void gui_end()
{
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void gui_draw_window_info(gui_state_t& state, float fps)
{
	ImGui::PushID("gui_draw_window_info");
	ImGui::Text("Renderer: %s", state.window->renderer_str.c_str());
	ImGui::Text("GL Version: %s", state.window->version_str.c_str());
	ImGui::Text("GLSL Version: %s", state.window->glsl_str.c_str());
	ImGui::Text("FPS: %.0f", fps);
	ImGui::PopID();
}

void gui_draw_transform(transform_t& transform)
{
	ImGui::PushID("gui_draw_transform");
	ImGui::PushID(&transform);
	ImGui::PushItemWidth(state.window->dimensions.x/8);
	ImGui::InputFloat3("Scale", glm::value_ptr(transform.scale));
	ImGui::InputFloat3("Position", glm::value_ptr(transform.position));
	glm::vec3 euler_angles = glm::eulerAngles(transform.orientation);
	ImGui::InputFloat3("Orientation", glm::value_ptr(euler_angles));
	transform.orientation = glm::quat(euler_angles);
	ImGui::PopItemWidth();
	ImGui::PopID();
	ImGui::PopID();
}

void gui_draw_scene_info(gui_state_t& state)
{
	ImGui::PushID("gui_draw_scene_info");
	ImGui::Text("Capacity: %d", state.scene->capacity);
	ImGui::Text("Size: %d", state.scene->size);

	if(ImGui::TreeNode("Entities"))
	{
		for(int i = 0; i < state.scene->size; i++)
		{
			if(ImGui::TreeNode(state.scene->names[i].c_str()))
			{
				gui_draw_transform(state.scene->transforms[i]);
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	ImGui::PopID();
}
