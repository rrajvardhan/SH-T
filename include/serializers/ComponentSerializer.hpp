#pragma once

#include "CameraComponents.hpp"
#include "CollisionComponents.hpp"
#include "JsonSerializer.hpp"
#include "PhysicsComponents.hpp"
#include "RenderableComponents.hpp"
#include "SpriteComponents.hpp"
#include "UtilComponents.hpp"

namespace ComponentSerializer
{
void
Serialize(JSONSerializer& serializer, const Transform& t);
void
Serialize(JSONSerializer& serializer, const RigidBody& rb);
void
Serialize(JSONSerializer& s, const Collider& c);
void
Serialize(JSONSerializer& s, const FollowCamera& cam);
void
Serialize(JSONSerializer& s, const Force& f);
void
Serialize(JSONSerializer& s, const Renderable& r);
void
Serialize(JSONSerializer& s, const Sprite& sp);
void
Serialize(JSONSerializer& s, const Identification& id);
void
Serialize(JSONSerializer& s, const SpriteAnimator& animation);

void
Deserialize(const JSON& j, Transform& t);
void
Deserialize(const JSON& j, RigidBody& rb);
void
Deserialize(const JSON& j, Collider& c);
void
Deserialize(const JSON& j, FollowCamera& cam);
void
Deserialize(const JSON& j, Force& f);
void
Deserialize(const JSON& j, Renderable& r);
void
Deserialize(const JSON& j, Sprite& sp);
void
Deserialize(const JSON& j, Identification& id);
void
Deserialize(const JSON& j, SpriteAnimator& animation);
}
