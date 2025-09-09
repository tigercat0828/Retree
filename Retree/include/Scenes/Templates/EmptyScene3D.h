#pragma once

#include "Core/Brush3D.h"
#include <imgui/imgui.h>
#include <glad/glad.h>
#include "Core/RenderTarget.h"
#include "Core/Scene.h"
#include "Core/Window.h"
#include "Core/Skybox.h"
using namespace glm;
class EmptyScene3D : public Scene {
private:	// global
	InputManager& Input = InputManager::Instance();
	Window& window = Window::Instance();
private:	// for imgui scene view window
	ImVec2 ImAnchor;
	vec2 ImCursorOffset;
	int ImWidth{ 800 };
	int ImHeight{ 600 };
	RenderTarget ImRenderTarget;
private: // Assets
	Camera Acam;
	Skybox Askybox;
	Brush3D Abrush;
public:
	EmptyScene3D() = default;

	void OnLoad() override {
		ImRenderTarget = RenderTarget(800, 600);
		Acam.LookAtFrom(vec3(0), vec3(5));

	}
	void OnUpdate(float deltaTime) override {
		if (window.GetCursorMode() == CursorMode::CURSOR_LOCKED) {
			Acam.OnUpdate(deltaTime);
		}
	}
	void OnDraw() override {

		ImRenderTarget.Bind();	// draw to imgui texture
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, ImWidth, ImHeight);
		Abrush.Begin(Acam);
		Abrush.DrawAxis();
		Askybox.OnDraw(Acam);
		
		ImRenderTarget.Unbind();
	}
	void OnEvent(const SDL_Event& event) override {}
	void OnExit() override {}

	void OnImGui() override {
		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
		Im_ShowMenuBar();
		Im_SceneView();

	}
	void Im_ShowMenuBar() {
		ImGui::BeginMainMenuBar();
		ImAnchor = ImGui::GetWindowPos();
		if (ImGui::BeginMenu("Menu")) {
			if (ImGui::MenuItem("Exit", "Alt+F4")) { window.Close(); }
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	void Im_SceneView() {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Scene");
		auto anchor = ImGui::GetCursorScreenPos();
		ImCursorOffset = vec2(anchor.x - ImAnchor.x, anchor.y - ImAnchor.y);

		const float width = ImGui::GetContentRegionAvail().x;
		const float height = ImGui::GetContentRegionAvail().y;
		ImWidth = width;
		ImHeight = height;
		ImRenderTarget.Rescale(width, height);
		Acam.SetAspectRatio(width, height);

		ImGui::GetWindowDrawList()->AddImage(
			(ImTextureID)(intptr_t)ImRenderTarget.GetTextureID(),
			ImVec2(anchor.x, anchor.y),
			ImVec2(anchor.x + width, anchor.y + height),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);
		ImGui::End();
		ImGui::PopStyleVar();
	}
};
