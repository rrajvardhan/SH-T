#include "ComponentSerializer.hpp"
#include "Json.hpp"
#include "Log.hpp"
#include "Overseer.hpp"
#include "SceneManager.hpp"
#include "SpriteComponents.hpp"

SceneManager::SceneManager(Overseer& ecs) : _ecs(ecs)
{
}

SceneManager::~SceneManager()
{
}

bool
SceneManager::addScene(const std::string& path)
{

  if (_sceneCache.find(path) != _sceneCache.end())
    return true;

  std::ifstream inFile(path);
  if (!inFile.is_open())
  {
    LOG_DEBUG(" [SceneManager] Failed to open scene file: ", path);
    return false;
  }

  JSON root;
  inFile >> root;

  if (!root.contains("entities") || !root["entities"].is_array())
  {
    LOG_ERROR("[SceneManager] Scene file missing 'entities' array: ", path);
    return false;
  }

  Scene scene;
  scene.path = path;
  scene.name = root.value("name", "<unnamed>");
  scene.data = root;

  _sceneCache[scene.name] = scene;

  LOG_INFO("[SceneManager] Scene '", scene.name, "' added to cache.");
  return true;
}

void
SceneManager::loadScene(const std::string& name)
{
  if (_sceneCache.find(name) == _sceneCache.end())
  {
    LOG_ERROR("[SceneManager] Scene '", name, "' not found in cache.");
    return;
  }

  _ecs.clearEntities();

  const Scene& scene = _sceneCache.at(name);
  const JSON&  root  = scene.data;

  for (const auto& entityJson : root["entities"])
  {
    Entity e = _ecs.createEntity();

    if (entityJson.contains("Transform"))
    {
      Transform t;
      ComponentSerializer::Deserialize(entityJson["Transform"], t);
      _ecs.addComponent(e, t);
    }

    if (entityJson.contains("RigidBody"))
    {
      RigidBody rb;
      ComponentSerializer::Deserialize(entityJson["RigidBody"], rb);
      _ecs.addComponent(e, rb);
    }

    if (entityJson.contains("Collider"))
    {
      Collider c;
      ComponentSerializer::Deserialize(entityJson["Collider"], c);
      _ecs.addComponent(e, c);
    }

    if (entityJson.contains("FollowCamera"))
    {
      FollowCamera cam;
      ComponentSerializer::Deserialize(entityJson["FollowCamera"], cam);
      _ecs.addComponent(e, cam);
    }

    if (entityJson.contains("Force"))
    {
      Force f;
      ComponentSerializer::Deserialize(entityJson["Force"], f);
      _ecs.addComponent(e, f);
    }

    if (entityJson.contains("Renderable"))
    {
      Renderable r;
      ComponentSerializer::Deserialize(entityJson["Renderable"], r);
      _ecs.addComponent(e, r);
    }

    if (entityJson.contains("Sprite"))
    {
      Sprite s;
      ComponentSerializer::Deserialize(entityJson["Sprite"], s);
      _ecs.addComponent(e, s);
    }

    if (entityJson.contains("Identification"))
    {
      Identification id;
      ComponentSerializer::Deserialize(entityJson["Identification"], id);
      _ecs.addComponent(e, id);
    }

    if (entityJson.contains("SpriteAnimator"))
    {
      SpriteAnimator sa;
      ComponentSerializer::Deserialize(entityJson["SpriteAnimator"], sa);
      _ecs.addComponent(e, sa);
    }
  }

  _currentScene = scene;

  LOG_INFO("[SceneManager] Scene '", root.value("name", "<unnamed>"), "' loaded.");
}

bool
SceneManager::unload(const std::string& name)
{
  auto it = _sceneCache.find(name);
  if (it == _sceneCache.end())
    return false;

  if (_currentScene.name == name)
    _currentScene = Scene{};

  _sceneCache.erase(it);
  LOG_INFO("[SceneManager] Scene '", name, "' removed from cache.");
  return true;
}
