#include "CameraComponents.hpp"
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
                   [&ecs](Entity entity) -> Transform*
                   { return &ecs.getComponent<Transform>(entity); });

  lua.set_function("add_transform", [&ecs](Entity e) { ecs.addComponent(e, Transform{}); });

  lua.set_function("remove_transform", [&ecs](Entity e) { ecs.removeComponent<Transform>(e); });
}

void
bindForce(sol::state& lua, Overseer& ecs)
{
  lua.new_usertype<Force>("Force", "vector", &Force::vector);

  lua.set_function("get_force",
                   [&ecs](Entity entity) -> Force* { return &ecs.getComponent<Force>(entity); });

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

  lua.set_function("bind_follow_camera_to",
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
} // namespace LuaBindings
