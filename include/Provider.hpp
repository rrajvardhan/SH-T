#pragma once

#include "ServiceContext.hpp"

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
