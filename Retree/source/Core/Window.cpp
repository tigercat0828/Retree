#include "pch.h"
#include "Core/Window.h"
#include "Core/ShaderManager.h"
#include "Core/Primitives.h"
Window::Window(const string& title, int width, int height) : mTitle(title), mWidth(width), mHeight(height) {
	mIsRunning = true;
	InitSDL();
	InitGL();
	InitImGui();
	Config();

	// initial system component
	ShaderManager::LoadAll();
	Primitive::SetupGL();
	// EnableCullFace();
	// glEnable(GL_PROGRAM_POINT_SIZE); // some drive allow it by default ?
}

Window& Window::Instance() {
	static Window mInstance("PlaceHolder", 1600, 900);
	return mInstance;
}

void Window::Run() {

	while (mIsRunning) {
		Input.UpdatePrev();

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL3_ProcessEvent(&event);
			OnEvent(event);
			Input.OnEvent(event);
			SceneManager.OnEvent(event);
		}

		// Update
		Input.Update();
		SceneManager.OnUpdate(mDeltaTime);

		// Rendering
		BeginFrame();
		SceneManager.OnImGui();
		SceneManager.OnDraw();
		EndFrame();

		float current = SDL_GetTicks() / 1000.0f;
		mDeltaTime = current - mLastTime;
		mLastTime = current;
		// Sleep
		if (mDeltaTime < TIME_PER_FRAME) {
			SDL_Delay(TIME_PER_FRAME - mDeltaTime);
		}
	}
	SceneManager.OnExit();
	OnExit();
}

void Window::Close() {
	mIsRunning = false;
}

void Window::LockCursor() {
	SDL_SetWindowRelativeMouseMode(mWindow, true);
	mCursorMode = CursorMode::CURSOR_LOCKED;
}

void Window::UnlockCursor() {
	SDL_SetWindowRelativeMouseMode(mWindow, false);
	mCursorMode = CursorMode::CURSOR_FREE;
}

void Window::OnEvent(const SDL_Event& event) {
	switch (event.type) {
	case SDL_EVENT_QUIT:
		Close();
		break;
	case SDL_EVENT_KEY_DOWN:
		if (event.key.key == SDLK_ESCAPE) Close();

		if (event.key.key == SDLK_TAB) {
			if (mCursorMode == CursorMode::CURSOR_FREE)
				LockCursor();
			else
				UnlockCursor();
		}
		break;
	case SDL_EVENT_WINDOW_RESIZED:
		SetWindowSize(event.window.data1, event.window.data2);
		break;
	}
}

void Window::OnExit() {
	
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
	SDL_GL_DestroyContext(mGLContext);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

void Window::InitImGui() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // TODO
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer bindings
	if (!ImGui_ImplSDL3_InitForOpenGL(mWindow, mGLContext)) {
		Logger::Error("Failed to init ImGui SDL2 binding");
		return;
	}
	if (!ImGui_ImplOpenGL3_Init("#version 460")) {
		Logger::Error("Failed to init ImGui OpenGL binding");
	}

}

void Window::InitSDL() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {		//  SDL_INIT_EVERYTHING
		Logger::Error("Failed to init SDL : {}", SDL_GetError());
		exit(-1);
	}
	else {
		Logger::Info("Success to init SDL");
	}
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	mWindow = SDL_CreateWindow(
		mTitle.c_str(), mWidth, mHeight,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);

	if (mWindow == NULL) {
		Logger::Error("Failed to create SDL window : {}", SDL_GetError());
		SDL_Quit();
		exit(-1);
	}
	else {
		Logger::Info("Success to create SDL window");
	}
}

void Window::InitGL() {
	mGLContext = SDL_GL_CreateContext(mWindow);
	if (!mGLContext) {
		Logger::Error("Failed to create OpenGL context : {}", SDL_GetError());
		SDL_DestroyWindow(mWindow);
		SDL_Quit();
		return;
	}
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		Logger::Error("Failed to load OpenGL process");
		SDL_Quit();
		exit(-1);
	}
	else {
		Logger::Info("Success to init GLAD");
	}
	SDL_Log("OpenGL version: %s", glGetString(GL_VERSION));
	SDL_Log("GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
	SDL_Log("Vendor: %s", glGetString(GL_VENDOR));
	SDL_Log("Renderer: %s", glGetString(GL_RENDERER));

	SDL_GL_MakeCurrent(mWindow, mGLContext);
	SDL_GL_SetSwapInterval(1); // Enable vsync
}

void Window::Config() {
	glClearColor(0, 0, 0, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	stbi_set_flip_vertically_on_load(false);
}

void Window::BeginFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
}

void Window::EndFrame() {
	// TODO 

	ImGui::Render(); // ´è¬V ImGui
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	ImGuiIO& io = ImGui::GetIO();
	// Update and Render additional Platform Windows
	// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	//  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}
	SDL_GL_SwapWindow(mWindow);
}

void Window::GetWindowSize(int& width, int& height) const {

	SDL_GetWindowSize(mWindow, &width, &height);
}

void Window::SetWindowSize(int width, int height) {
	mWidth = width;
	mHeight = height;
	SDL_SetWindowSize(mWindow, width, height);
	glViewport(0, 0, mWidth, mHeight);
}

CursorMode Window::GetCursorMode() const {
	return mCursorMode;
}

void Window::SetWindowTitle(const char* title) {
	mTitle = title;
	SDL_SetWindowTitle(mWindow, title);
}

void Window::SetStartScene(std::unique_ptr<Scene> scene) {
	SceneManager.SetStartScene(std::move(scene));
}

float Window::GetTime() const { return mLastTime; }

