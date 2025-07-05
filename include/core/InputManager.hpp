#pragma once

#include "Vector2D.hpp"
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_stdinc.h>

class InputManager
{

public:
  InputManager();
  ~InputManager();

  enum MOUSE_BUTTON
  {
    left = 0,
    right,
    middle,
    back,
    forward
  };

  bool keyDown(SDL_Scancode scancode);
  bool keyPressed(SDL_Scancode scancode);
  bool keyReleased(SDL_Scancode scancode);

  bool mouseDown(MOUSE_BUTTON button);
  bool mousePressed(MOUSE_BUTTON button);
  bool mouseReleased(MOUSE_BUTTON button);

  Vector2D mousePos();

  void update();
  void updatePrev();

private:
  const Uint8* _keyboardState;
  Uint8*       _prevKeyboardState;
  int          _keyLength;

  Uint32 _prevMouseState;
  Uint32 _mouseState;

  int _mouseX;
  int _mouseY;
};
