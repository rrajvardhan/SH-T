#include "CameraComponents.hpp"
#include "CollisionComponents.hpp"
#include "InputManager.hpp"
#include "LuaBindings.hpp"
#include "Overseer.hpp"
#include "PhysicsComponents.hpp"
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

  lua.new_usertype<FollowCamera>(
      "FollowCamera", "target", &FollowCamera::target, "isActive", &FollowCamera::isActive);

  lua.set_function("get_follow_camera",
                   [&ecs](Entity e) -> FollowCamera*
                   { return &ecs.getComponent<FollowCamera>(e); });

  lua.set_function("add_follow_camera",
                   [&ecs](Entity e) { ecs.addComponent(e, FollowCamera{ e, true }); });

  lua.set_function("remove_follow_camera",
                   [&ecs](Entity e) { ecs.removeComponent<FollowCamera>(e); });

  lua.set_function("set_follow_camera_target",
                   [&ecs](Entity cam, Entity target)
                   {
                     auto& fc    = ecs.getComponent<FollowCamera>(cam);
                     fc.target   = target;
                     fc.isActive = true;
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
  lua.set("KEY_A", SDL_SCANCODE_A);
  lua.set("KEY_D", SDL_SCANCODE_D);
  lua.set("KEY_W", SDL_SCANCODE_W);
  lua.set("KEY_S", SDL_SCANCODE_S);
  lua.set("KEY_SPACE", SDL_SCANCODE_SPACE);
  lua.set("KEY_LEFT", SDL_SCANCODE_LEFT);
  lua.set("KEY_RIGHT", SDL_SCANCODE_RIGHT);
  lua.set("KEY_UP", SDL_SCANCODE_UP);
  lua.set("KEY_DOWN", SDL_SCANCODE_DOWN);
  lua.set("KEY_ESCAPE", SDL_SCANCODE_ESCAPE);
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

} // namespace LuaBindings
