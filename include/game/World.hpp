#pragma once

#include "Overseer.hpp"
#include "ServiceContext.hpp"
#include "Vector2D.hpp"

struct WorldProvider
{
  ServiceContext& service;
  Vector2D&       cameraOffset;

  int vw;
  int vh;

  WorldProvider(ServiceContext& ctxRef, Vector2D& camOffsetRef)
      : service(ctxRef), cameraOffset(camOffsetRef)
  {
  }

  const Vector2D& getCameraOffset() const { return cameraOffset; }
  void            setCameraOffset(const Vector2D& offset) { cameraOffset = offset; }
};

class World
{
public:
  World(ServiceContext& ctx);
  ~World();

  bool init();
  void update();
  void render();

  WorldProvider& getProvider() { return _provider; }
  void           setViewport(int width, int height)
  {
    _provider.vw = width;
    _provider.vh = height;
  }

  Overseer& getECS() { return ecs; }

private:
  Overseer ecs;
  Vector2D _cameraOffset;

  ServiceContext& _ctx;
  WorldProvider   _provider;
};
