#pragma once
#include <memory>
#include "Core/Scene.h"
using std::unique_ptr;
// The Meyers Singleton
// TODO : store scenes with stack

class SceneManager {
private:
	~SceneManager() = default;
	SceneManager() = default;
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;
private:
	unique_ptr<Scene> CurrentScene;
public:
	static SceneManager& Instance();
	void SetStartScene(unique_ptr<Scene> scene);
	void ChangeScene(unique_ptr<Scene> scene);

	void OnUpdate(float deltaTime);
	void OnDraw();
	void OnImGui();
	void OnEvent(const SDL_Event& event);
	void OnExit();
};