#include "pch.h"
#include "Core/SceneManager.h"

SceneManager& SceneManager::Instance() {
	static SceneManager instance;
	return instance;
}

void SceneManager::SetStartScene(unique_ptr<Scene> scene) {
	CurrentScene = std::move(scene);
	CurrentScene->OnLoad();
}

void SceneManager::ChangeScene(unique_ptr<Scene> scene) {
	if (CurrentScene) CurrentScene->OnExit();
	CurrentScene = std::move(scene);
	CurrentScene->OnLoad();
}

void SceneManager::OnUpdate(float deltaTime) {
	CurrentScene->OnUpdate(deltaTime);
}

void SceneManager::OnDraw() {
	CurrentScene->OnDraw();
}

void SceneManager::OnImGui() {
	CurrentScene->OnImGui();
}

void SceneManager::OnEvent(const SDL_Event& event) {
	CurrentScene->OnEvent(event);
}

void SceneManager::OnExit() {
	CurrentScene->OnExit();
}
