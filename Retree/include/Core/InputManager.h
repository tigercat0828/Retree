#pragma once
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <memory>
// The Meyers Singleton
using glm::vec2;
using std::unique_ptr;
class InputManager {
private:
    ~InputManager() {

    }
    InputManager();
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;
public:
    static InputManager& Instance() {
        static InputManager m_instance;
        return m_instance;
    }
private:
    float mMousePosX{ 0 };
    float mMousePosY{ 0 };
    int mMouseOffsetX{ 0 };
    int mMouseOffsetY{ 0 };
    float mMouseWheelY{ 0 };

    const bool* mCurrentKeyboardState;
    unique_ptr<bool[]> mPreviousKeyboardState;

    Uint32 mCurrentMouseState{ 0 };
    Uint32 mPreviousMouseState{ 0 };
public:
    void UpdatePrev();
    void OnEvent(const SDL_Event& event);
    void Update();
    bool GetKeyPressed(SDL_Scancode scancode) const;
    bool GetKeyReleased(SDL_Scancode scancode) const;
    bool GetKeyDown(SDL_Scancode scancode) const;
    bool GetKeyUp(SDL_Scancode scancode) const;


    /// <summary>
    /// SDL_BUTTON_LEFT, SDL_BUTTON_MIDDLE, SDL_BUTTON_RIGHT
    /// </summary>
    bool GetMouseButtonDown(int button) const;
    bool GetMouseButtonUp(int button) const;
    bool GetMouseButtonPressed(int button) const;
    bool GetMouseButtonReleased(int button) const;
    vec2 GetMousePos() const;
    vec2 GetMouseOffset() const;
    float GetMouseWheelY()const;
};