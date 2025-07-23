#include "CameraComponents.hpp"
#include "ComponentSerializer.hpp"
#include "JsonSerializer.hpp"
#include "RenderableComponents.hpp"

namespace ComponentSerializer
{
void
Serialize(JSONSerializer& s, const Transform& t)
{
  s.StartNewObject("Transform")
      .StartNewObject("position")
      .AddKeyValuePair("x", t.position.x)
      .AddKeyValuePair("y", t.position.y)
      .EndObject()
      .AddKeyValuePair("rotation", t.rotation)
      .EndObject();
}

void
Serialize(JSONSerializer& s, const RigidBody& rb)
{
  s.StartNewObject("RigidBody")
      .StartNewObject("velocity")
      .AddKeyValuePair("x", rb.velocity.x)
      .AddKeyValuePair("y", rb.velocity.y)
      .EndObject()
      .StartNewObject("acceleration")
      .AddKeyValuePair("x", rb.acceleration.x)
      .AddKeyValuePair("y", rb.acceleration.y)
      .EndObject()
      .AddKeyValuePair("mass", rb.mass)
      .EndObject();
}

void
Serialize(JSONSerializer& s, const Collider& c)
{
  s.StartNewObject("Collider")
      .StartNewObject("size")
      .AddKeyValuePair("x", c.size.x)
      .AddKeyValuePair("y", c.size.y)
      .EndObject()
      .StartNewObject("offset")
      .AddKeyValuePair("x", c.offset.x)
      .AddKeyValuePair("y", c.offset.y)
      .EndObject()
      .AddKeyValuePair("static", c.isStatic)
      .EndObject();
}

void
Serialize(JSONSerializer& s, const FollowCamera& cam)
{
  s.StartNewObject("FollowCamera").AddKeyValuePair("active", cam.isActive).EndObject();
}

void
Serialize(JSONSerializer& s, const Renderable& r)
{

  s.StartNewObject("Renderable")
      .StartNewObject("size")
      .AddKeyValuePair("x", r.size.x)
      .AddKeyValuePair("y", r.size.y)
      .EndObject()
      .StartNewObject("color")
      .AddKeyValuePair("r", r.color.r)
      .AddKeyValuePair("g", r.color.g)
      .AddKeyValuePair("b", r.color.b)
      .AddKeyValuePair("a", r.color.a)
      .EndObject()
      .AddKeyValuePair("shape", r.shape == Renderable::RECT ? "RECT" : "CIRCLE")
      .EndObject();
}

void
Serialize(JSONSerializer& s, const Force& f)
{
  s.StartNewObject("Force")
      .StartNewObject("vector")
      .AddKeyValuePair("x", f.vector.x)
      .AddKeyValuePair("y", f.vector.y)
      .EndObject()
      .EndObject();
}

void
Serialize(JSONSerializer& s, const Sprite& sp)
{
  s.StartNewObject("Sprite")
      .AddKeyValuePair("name", sp.name)
      .StartNewObject("Rect")
      .AddKeyValuePair("x", sp.srcRect.x)
      .AddKeyValuePair("y", sp.srcRect.y)
      .AddKeyValuePair("w", sp.srcRect.w)
      .AddKeyValuePair("h", sp.srcRect.h)
      .EndObject()
      .AddKeyValuePair("scale", sp.scale)
      .AddKeyValuePair("flip", sp.flip)
      .StartNewObject("offset")
      .AddKeyValuePair("x", sp.offset.x)
      .AddKeyValuePair("y", sp.offset.y)
      .EndObject()
      .AddKeyValuePair("layer", sp.layer)
      .AddKeyValuePair("blend mode", sp.blendMode)
      .EndObject();
}

void
Serialize(JSONSerializer& s, const Identification& id)
{
  s.StartNewObject("Identification")
      .AddKeyValuePair("name", id.name)
      .AddKeyValuePair("group", id.group)
      .EndObject();
}

void
Deserialize(const JSON& j, Transform& t)
{
  t.position.x = j["position"]["x"];
  t.position.y = j["position"]["y"];
  t.rotation   = j["rotation"];
}

void
Deserialize(const JSON& j, RigidBody& rb)
{
  rb.velocity.x     = j["velocity"]["x"];
  rb.velocity.y     = j["velocity"]["y"];
  rb.acceleration.x = j["acceleration"]["x"];
  rb.acceleration.y = j["acceleration"]["y"];
  rb.mass           = j["mass"];
}

void
Deserialize(const JSON& j, Collider& c)
{
  c.size.x   = j["size"]["x"];
  c.size.y   = j["size"]["y"];
  c.offset.x = j["offset"]["x"];
  c.offset.y = j["offset"]["y"];
  c.isStatic = j["static"];
}

void
Deserialize(const JSON& j, FollowCamera& cam)
{
  cam.isActive = j["active"];
}

void
Deserialize(const JSON& j, Force& f)
{
  f.vector.x = j["vector"]["x"];
  f.vector.y = j["vector"]["y"];
}

void
Deserialize(const JSON& j, Renderable& r)
{
  r.size.x = j["size"]["x"];
  r.size.y = j["size"]["y"];

  r.color.r = j["color"]["r"];
  r.color.g = j["color"]["g"];
  r.color.b = j["color"]["b"];
  r.color.a = j["color"]["a"];

  std::string shape = j.value("shape", "RECT");
  r.shape           = (shape == "CIRCLE") ? Renderable::CIRCLE : Renderable::RECT;
}

void
Deserialize(const JSON& j, Sprite& s)
{
  s.name      = j["name"];
  s.srcRect.x = j["Rect"]["x"];
  s.srcRect.y = j["Rect"]["y"];
  s.srcRect.w = j["Rect"]["w"];
  s.srcRect.h = j["Rect"]["h"];
  s.scale     = j["scale"];
  s.flip      = j["flip"];
  s.offset.x  = j["offset"]["x"];
  s.offset.y  = j["offset"]["y"];
  s.layer     = j["layer"];
  s.blendMode = j["blend mode"];
}

void
Deserialize(const JSON& j, Identification& id)
{
  id.name  = j["name"];
  id.group = j["group"];
}
}
