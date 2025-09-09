#pragma once
#include "imgui.h"
#include <glad/glad.h> 
#include "Core/Scene.h"
#include "Core/InputManager.h"
#include "Core/Window.h"
#include "Core/RenderTarget.h"

using namespace glm;
class EmptySceneVoid : public Scene {
	InputManager& Input = InputManager::Instance();
	Window& window = Window::Instance();
private:
	ImVec2 originAnchor;
	vec2 cursorOffset;
    int mSceneWidth{ 800 };
	int mSceneHeight{ 600 };
private: // Assets
	RenderTarget renderTarget;
public:
	EmptySceneVoid() = default;

	void OnLoad() override {
		renderTarget = RenderTarget(800, 600);
	}
	void OnUpdate(float deltaTime) override {
	}
	void OnDraw() override {

		renderTarget.Bind();	// draw to imgui texture
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// draw something
		renderTarget.Unbind();
	}
	void OnImGui() override {
		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
		Im_ShowMenuBar();
		Im_SceneView();
	
	}
	void OnEvent(const SDL_Event& event) override {}
	void OnExit() override {}

	void Im_ShowMenuBar() {
		ImGui::BeginMainMenuBar();
		originAnchor = ImGui::GetWindowPos();
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
		cursorOffset = vec2(anchor.x - originAnchor.x, anchor.y - originAnchor.y);

		const float width = ImGui::GetContentRegionAvail().x;
		const float height = ImGui::GetContentRegionAvail().y;
		mSceneWidth = width;
        mSceneHeight = height;
		renderTarget.Rescale(width, height);

		ImGui::GetWindowDrawList()->AddImage(
			(ImTextureID)(intptr_t)renderTarget.GetTextureID(),
			ImVec2(anchor.x, anchor.y),
			ImVec2(anchor.x + width, anchor.y + height),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);
		ImGui::End();
		ImGui::PopStyleVar();
	}
};
