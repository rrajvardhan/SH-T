#include "AudioManager.hpp"
#include "CameraComponents.hpp"
#include "CollisionComponents.hpp"
#include "InputManager.hpp"
#include "LuaBindings.hpp"
#include "Overseer.hpp"
#include "PhysicsComponents.hpp"
#include "Timer.hpp"
#include "Types.hpp"
#include "Vector2D.hpp"

namespace LuaBindings
{

void
bindVector2D(sol::state& lua)
{
  lua.new_usertype<Vector2D>("Vector2D",
                             sol::constructors<Vector2D(), Vector2D(float, float)>(),
                             "x",
                             &Vector2D::x,
                             "y",
                             &Vector2D::y);
}

void
bindTransform(sol::state& lua, Overseer& ecs)
{

  lua.new_usertype<Transform>("Transform",
                              sol::constructors<Transform()>(),
                              "position",
                              &Transform::position,
                              "rotation",
                              &Transform::rotation);

  lua.set_function("get_transform",
                   [&ecs](Entity e) -> Transform* { return &ecs.getComponent<Transform>(e); });

  lua.set_function("add_transform", [&ecs](Entity e) { ecs.addComponent(e, Transform{}); });

  lua.set_function("remove_transform", [&ecs](Entity e) { ecs.removeComponent<Transform>(e); });
}

void
bindForce(sol::state& lua, Overseer& ecs)
{
  lua.new_usertype<Force>("Force", "vector", &Force::vector);

  lua.set_function("get_force", [&ecs](Entity e) -> Force* { return &ecs.getComponent<Force>(e); });

  lua.set_function("add_force", [&ecs](Entity e) { ecs.addComponent(e, Force{}); });

  lua.set_function("remove_force", [&ecs](Entity e) { ecs.removeComponent<Force>(e); });
}

void
bindFollowCamera(sol::state& lua, Overseer& ecs)
{

  lua.new_usertype<FollowCamera>("FollowCamera", "isActive", &FollowCamera::isActive);

  lua.set_function("get_follow_camera",
                   [&ecs](Entity e) -> FollowCamera*
                   { return &ecs.getComponent<FollowCamera>(e); });

  lua.set_function("add_follow_camera",
                   [&ecs](Entity e) { ecs.addComponent(e, FollowCamera{ true }); });

  lua.set_function("remove_follow_camera",
                   [&ecs](Entity e) { ecs.removeComponent<FollowCamera>(e); });

  lua.set_function("set_follow_camera",
                   [&ecs](Entity entity, bool state)
                   {
                     auto& fc    = ecs.getComponent<FollowCamera>(entity);
                     fc.isActive = state;
                   });

  lua.set_function("create_follow_camera",
                   [&ecs]()
                   {
                     Entity cam = ecs.createEntity();
                     ecs.addComponent(cam, FollowCamera{});
                     ecs.addComponent(cam, Transform{});

                     return cam;
                   });
}

void
bindCollider(sol::state& lua, Overseer& ecs)
{

  lua.new_usertype<Collider>("Collider",
                             "size",
                             &Collider::size,
                             "offset",
                             &Collider::offset,
                             "static",
                             &Collider::isStatic);

  lua.set_function("get_collider",
                   [&ecs](Entity e) -> Collider* { return &ecs.getComponent<Collider>(e); });

  lua.set_function("add_collider", [&ecs](Entity e) { ecs.addComponent(e, Collider{}); });

  lua.set_function("remove_collider", [&ecs](Entity e) { ecs.removeComponent<Collider>(e); });
}

void
bindRigidBody(sol::state& lua, Overseer& ecs)
{
  lua.new_usertype<RigidBody>("RigidBody",
                              "velocity",
                              &RigidBody::velocity,
                              "acceleration",
                              &RigidBody::acceleration,
                              "mass",
                              &RigidBody::mass);

  lua.set_function("get_rigidbody",
                   [&ecs](Entity e) -> RigidBody* { return &ecs.getComponent<RigidBody>(e); });

  lua.set_function("add_rigidbody", [&ecs](Entity e) { ecs.addComponent(e, RigidBody{}); });

  lua.set_function("remove_rigidbody", [&ecs](Entity e) { ecs.removeComponent<RigidBody>(e); });
}

void
bindKeyConstants(sol::state& lua)
{
  // A-Z
  for (char c = 'A'; c <= 'Z'; ++c)
  {
    std::string key = "KEY_";
    key += c;
    lua.set(key, SDL_SCANCODE_A + (c - 'A'));
  }

  // 0–9
  for (char c = '0'; c <= '9'; ++c)
  {
    std::string key = "KEY_";
    key += c;
    lua.set(key, SDL_SCANCODE_0 + (c - '0'));
  }

  // Arrow keys
  lua.set("KEY_LEFT", SDL_SCANCODE_LEFT);
  lua.set("KEY_RIGHT", SDL_SCANCODE_RIGHT);
  lua.set("KEY_UP", SDL_SCANCODE_UP);
  lua.set("KEY_DOWN", SDL_SCANCODE_DOWN);

  // Whitespace and control
  lua.set("KEY_SPACE", SDL_SCANCODE_SPACE);
  lua.set("KEY_TAB", SDL_SCANCODE_TAB);
  lua.set("KEY_ENTER", SDL_SCANCODE_RETURN);
  lua.set("KEY_ESCAPE", SDL_SCANCODE_ESCAPE);
  lua.set("KEY_BACKSPACE", SDL_SCANCODE_BACKSPACE);

  // Modifier keys
  lua.set("KEY_LSHIFT", SDL_SCANCODE_LSHIFT);
  lua.set("KEY_RSHIFT", SDL_SCANCODE_RSHIFT);
  lua.set("KEY_LCTRL", SDL_SCANCODE_LCTRL);
  lua.set("KEY_RCTRL", SDL_SCANCODE_RCTRL);
  lua.set("KEY_LALT", SDL_SCANCODE_LALT);
  lua.set("KEY_RALT", SDL_SCANCODE_RALT);

  // Common symbols (US QWERTY)
  lua.set("KEY_MINUS", SDL_SCANCODE_MINUS);               // -
  lua.set("KEY_EQUALS", SDL_SCANCODE_EQUALS);             // =
  lua.set("KEY_LEFTBRACKET", SDL_SCANCODE_LEFTBRACKET);   // [
  lua.set("KEY_RIGHTBRACKET", SDL_SCANCODE_RIGHTBRACKET); // ]
  lua.set("KEY_BACKSLASH", SDL_SCANCODE_BACKSLASH);       // "\"
  lua.set("KEY_SEMICOLON", SDL_SCANCODE_SEMICOLON);       // ;
  lua.set("KEY_APOSTROPHE", SDL_SCANCODE_APOSTROPHE);     // '
  lua.set("KEY_GRAVE", SDL_SCANCODE_GRAVE);               // `
  lua.set("KEY_COMMA", SDL_SCANCODE_COMMA);               // ,
  lua.set("KEY_PERIOD", SDL_SCANCODE_PERIOD);             // .
  lua.set("KEY_SLASH", SDL_SCANCODE_SLASH);               // /

  // Function keys
  for (int i = 1; i <= 12; ++i)
  {
    std::string key = "KEY_F" + std::to_string(i);
    lua.set(key, SDL_SCANCODE_F1 + (i - 1));
  }
}

void
bindInput(sol::state& lua, InputManager& input)
{

  lua.new_usertype<InputManager>("InputManager",
                                 "key_down",
                                 &InputManager::keyDown,
                                 "key_pressed",
                                 &InputManager::keyPressed,
                                 "key_released",
                                 &InputManager::keyReleased);
  lua["Input"] = &input;
}

void
bindTimer(sol::state& lua, Timer& timer)
{

  lua.new_usertype<Timer>("Timer", "delta_time", &Timer::getDeltaTime);
  lua["Timer"] = &timer;
}

void
bindAudio(sol::state& lua, AudioManager& audio)
{
  lua.new_usertype<AudioManager>("AudioManager",
                                 // Music
                                 "add_music",
                                 &AudioManager::addMusic,
                                 "play_music",
                                 &AudioManager::playMusic,
                                 "stop_music",
                                 &AudioManager::stopMusic,
                                 "pause_music",
                                 &AudioManager::pauseMusic,
                                 "resume_music",
                                 &AudioManager::resumeMusic,
                                 "unload_music",
                                 &AudioManager::unloadMusic,
                                 "has_music",
                                 &AudioManager::hasMusic,

                                 // SFX
                                 "add_sfx",
                                 &AudioManager::addSFX,
                                 "play_sfx",
                                 &AudioManager::playSFX,
                                 "unload_sfx",
                                 &AudioManager::unloadSFX,
                                 "has_sfx",
                                 &AudioManager::hasSFX,

                                 // Utility
                                 "rename",
                                 &AudioManager::renameAudio);
  lua["Audio"] = &audio;
}

} // namespace LuaBindings
