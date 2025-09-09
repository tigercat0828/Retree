#pragma once
union SDL_Event;
class Scene {
public:
	Scene() = default;
	virtual ~Scene() = default;
	virtual void OnLoad() = 0;
	virtual void OnEvent(const SDL_Event& event) = 0;
	virtual void OnUpdate(float deltaTime) = 0;
	virtual void OnImGui() = 0;
	virtual void OnDraw() = 0;
	virtual void OnExit() = 0;
};
