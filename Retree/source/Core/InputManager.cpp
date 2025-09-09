#include "pch.h"

#include "Core/InputManager.h"

InputManager::InputManager() {
    mCurrentKeyboardState = SDL_GetKeyboardState(nullptr);
    mPreviousKeyboardState = std::make_unique<bool[]>(SDL_SCANCODE_COUNT);
    memset(mPreviousKeyboardState.get(), 0, SDL_SCANCODE_COUNT * sizeof(bool));
}

void InputManager::UpdatePrev() {
    mMouseWheelY = 0;
    mMouseOffsetX = 0;
    mMouseOffsetY = 0;
    mPreviousMouseState = mCurrentMouseState;
    memcpy(mPreviousKeyboardState.get(), mCurrentKeyboardState, SDL_SCANCODE_COUNT * sizeof(Uint8));
}

void InputManager::OnEvent(const SDL_Event& event) {
    switch (event.type) {
    case SDL_EVENT_MOUSE_MOTION:
        mMouseOffsetX = event.motion.xrel;
        mMouseOffsetY = event.motion.yrel;
        break;
    case SDL_EVENT_MOUSE_WHEEL:
        mMouseWheelY = event.wheel.y;
        break;
    }
}

void InputManager::Update() {
    mCurrentMouseState = SDL_GetMouseState(&mMousePosX, &mMousePosY);
    mCurrentKeyboardState = SDL_GetKeyboardState(nullptr);
}

bool InputManager::GetKeyPressed(SDL_Scancode scancode) const {
    return mCurrentKeyboardState[scancode] != 0;
}

bool InputManager::GetKeyReleased(SDL_Scancode scancode) const {
    return mCurrentKeyboardState[scancode] == 0;
}

bool InputManager::GetKeyDown(SDL_Scancode scancode) const {
    return (mCurrentKeyboardState[scancode] != 0) && (mPreviousKeyboardState[scancode] == 0);
}

bool InputManager::GetKeyUp(SDL_Scancode scancode) const {
    return (mCurrentKeyboardState[scancode] == 0) && (mPreviousKeyboardState[scancode] != 0);
}

bool InputManager::GetMouseButtonDown(int button) const {
    return (mCurrentMouseState & SDL_BUTTON_MASK(button)) && !(mPreviousMouseState & SDL_BUTTON_MASK(button));
}


bool InputManager::GetMouseButtonUp(int button) const {
    return !(mCurrentMouseState & SDL_BUTTON_MASK(button)) && (mPreviousMouseState & SDL_BUTTON_MASK(button));
}

bool InputManager::GetMouseButtonPressed(int button) const {
    return mCurrentMouseState & SDL_BUTTON_MASK(button);
}

bool InputManager::GetMouseButtonReleased(int button) const {
    return !(mCurrentMouseState & SDL_BUTTON_MASK(button));
}

glm::vec2 InputManager::GetMousePos() const {
    return glm::vec2(mMousePosX, mMousePosY);
}

glm::vec2 InputManager::GetMouseOffset()const {
    return glm::vec2(mMouseOffsetX, mMouseOffsetY);
}

float InputManager::GetMouseWheelY() const {
    return mMouseWheelY;
}
