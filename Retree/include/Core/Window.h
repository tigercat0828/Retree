#pragma once
#include <memory>
#include <string>
#include <SDL3/SDL.h>
#include "Core/InputManager.h"
#include "Core/SceneManager.h"
using std::string;

enum class CursorMode {
	CURSOR_LOCKED, CURSOR_FREE
};
const int DEFAULT_WINDOW_WIDTH = 1600;
const int DEFAULT_WINDOW_HEIGHT = 900;

class Window {
private:
	~Window() {}
	Window() = default;
	Window(const string& title, int width, int height);
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
public:
	static Window& Instance();
private:
	const int FIXED_FPS = 144;
	const float TIME_PER_FRAME = 1000.0f / FIXED_FPS;
private:
	SDL_Window* mWindow = nullptr;
	SDL_GLContext mGLContext = nullptr;
	InputManager& Input = InputManager::Instance();
	SceneManager& SceneManager = SceneManager::Instance();
	string mTitle;
	uint32_t mWidth;
	uint32_t mHeight;
	bool mIsRunning = true;
	float mLastTime = 0;
	float mDeltaTime = 0;
	CursorMode mCursorMode = CursorMode::CURSOR_FREE;
public: // getter-setter
	void GetWindowSize(int& width, int& height) const;
	void SetWindowSize(int width, int height);
	CursorMode GetCursorMode() const;
	void SetWindowTitle(const char* title);
	void SetStartScene(std::unique_ptr<Scene> scene);
	float GetTime() const;
public:
	void Run();
	void Close();
	void LockCursor();
	void UnlockCursor();
private:
	void OnEvent(const SDL_Event& event);
	void OnUpdate(float deltaTime) {}
	void OnExit();
	void BeginFrame();
	void EndFrame();
private:
	void InitImGui();
	void InitSDL();
	void InitGL();
	void Config();
public:
// TODO move to GL state manager
	void EnableCullFace() {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	}
};