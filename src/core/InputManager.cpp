#include "InputManager.hpp"
#include <SDL2/SDL_mouse.h>

InputManager::InputManager() {
  _keyboardState = SDL_GetKeyboardState(&_keyLength);
  _prevKeyboardState = new Uint8[_keyLength];

  memcpy(_prevKeyboardState, _keyboardState, _keyLength);
}

InputManager::~InputManager() {
  delete[] _prevKeyboardState;
  _prevKeyboardState = nullptr;
}

bool InputManager::keyDown(SDL_Scancode scancode) { return _keyboardState[scancode]; }

bool InputManager::keyPressed(SDL_Scancode scancode) {
  return !_prevKeyboardState[scancode] && _keyboardState[scancode];
}

bool InputManager::keyReleased(SDL_Scancode scancode) {
  return _prevKeyboardState[scancode] && !_keyboardState[scancode];
}

Vector2D InputManager::mousePos() { return Vector2D((float)_mouseX, (float)_mouseY); }

bool InputManager::mouseDown(MOUSE_BUTTON button) {
  Uint32 mask = 0;

  switch (button) {
  case left:
    mask = SDL_BUTTON_LMASK;
    break;
  case right:
    mask = SDL_BUTTON_RMASK;
    break;
  case middle:
    mask = SDL_BUTTON_MMASK;
    break;
  case back:
    mask = SDL_BUTTON_X1MASK;
    break;
  case forward:
    mask = SDL_BUTTON_X2MASK;
    break;
  default:
    break;
  }

  return (_mouseState & mask);
}

bool InputManager::mousePressed(MOUSE_BUTTON button) {
  Uint32 mask = 0;

  switch (button) {
  case left:
    mask = SDL_BUTTON_LMASK;
    break;
  case right:
    mask = SDL_BUTTON_RMASK;
    break;
  case middle:
    mask = SDL_BUTTON_MMASK;
    break;
  case back:
    mask = SDL_BUTTON_X1MASK;
    break;
  case forward:
    mask = SDL_BUTTON_X2MASK;
    break;
  default:
    break;
  }

  return !(_prevMouseState & mask) && (_mouseState & mask);
}

bool InputManager::mouseReleased(MOUSE_BUTTON button) {
  Uint32 mask = 0;

  switch (button) {
  case left:
    mask = SDL_BUTTON_LMASK;
    break;
  case right:
    mask = SDL_BUTTON_RMASK;
    break;
  case middle:
    mask = SDL_BUTTON_MMASK;
    break;
  case back:
    mask = SDL_BUTTON_X1MASK;
    break;
  case forward:
    mask = SDL_BUTTON_X2MASK;
    break;
  default:
    break;
  }

  return (_prevMouseState & mask) && !(_mouseState & mask);
}

void InputManager::update() { _mouseState = SDL_GetMouseState(&_mouseX, &_mouseY); }

void InputManager::updatePrev() {

  memcpy(_prevKeyboardState, _keyboardState, _keyLength);
  _prevMouseState = _mouseState;
}
