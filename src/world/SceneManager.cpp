#include "ComponentSerializer.hpp"
#include "Json.hpp"
#include "Log.hpp"
#include "Overseer.hpp"
#include "SceneManager.hpp"
#include "SpriteComponents.hpp"
#include <iostream>

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

void
SceneManager::loadScene(const std::string& path, Overseer& ecs)
{
  clearAll(ecs);

  std::ifstream inFile(path);
  if (!inFile.is_open())
  {
    std::cerr << "Failed to open scene file: " << path << '\n';
    return;
  }

  JSON root;
  inFile >> root;

  if (!root.contains("entities") || !root["entities"].is_array())
  {
    std::cerr << "Scene file missing 'entities' array\n";
    return;
  }

  for (const auto& entityJson : root["entities"])
  {
    Entity e = ecs.createEntity();

    if (entityJson.contains("Transform"))
    {
      Transform t;
      ComponentSerializer::Deserialize(entityJson["Transform"], t);
      ecs.addComponent(e, t);
    }

    if (entityJson.contains("RigidBody"))
    {
      RigidBody rb;
      ComponentSerializer::Deserialize(entityJson["RigidBody"], rb);
      ecs.addComponent(e, rb);
    }

    if (entityJson.contains("Collider"))
    {
      Collider c;
      ComponentSerializer::Deserialize(entityJson["Collider"], c);
      ecs.addComponent(e, c);
    }

    if (entityJson.contains("FollowCamera"))
    {
      FollowCamera cam;
      ComponentSerializer::Deserialize(entityJson["FollowCamera"], cam);
      ecs.addComponent(e, cam);
    }

    if (entityJson.contains("Force"))
    {
      Force f;
      ComponentSerializer::Deserialize(entityJson["Force"], f);
      ecs.addComponent(e, f);
    }

    if (entityJson.contains("Renderable"))
    {
      Renderable r;
      ComponentSerializer::Deserialize(entityJson["Renderable"], r);
      ecs.addComponent(e, r);
    }

    if (entityJson.contains("Sprite"))
    {
      Sprite s;
      ComponentSerializer::Deserialize(entityJson["Sprite"], s);
      ecs.addComponent(e, s);
    }

    if (entityJson.contains("Identification"))
    {
      Identification id;
      ComponentSerializer::Deserialize(entityJson["Identification"], id);
      ecs.addComponent(e, id);
    }

    if (entityJson.contains("SpriteAnimator"))
    {
      SpriteAnimator sa;
      ComponentSerializer::Deserialize(entityJson["SpriteAnimator"], sa);
      ecs.addComponent(e, sa);
    }
  }

  _currentScene = path;

  LOG_INFO("Scene '", root.value("name", "<unnamed>"), "' loaded.\n");
}
