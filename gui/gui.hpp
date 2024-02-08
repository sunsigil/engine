#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "misc/cpp/imgui_stdlib.h"

struct gui_state_t
{
	int width;
	int height;
	ImGuiWindowFlags flags;

	std::string renderer_name;
	std::string renderer_version;
	float fps;
};

void gui_begin(gui_state_t& state)
{
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(state.width, state.height));
	ImGui::Begin("##window", nullptr, state.flags);
}

void gui_end()
{
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void gui_draw_info(gui_state_t& state)
{
	ImGui::Text("%s\n%s\n%.0f", state.renderer_name.c_str(), state.renderer_version.c_str(), state.fps);
}
