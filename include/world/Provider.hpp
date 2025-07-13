#pragma once

#include "Camera2D.hpp"
#include "ServiceContext.hpp"

struct WorldProvider
{
  ServiceContext& service;
  Camera2D&       camera;

  WorldProvider(ServiceContext& ctxRef, Camera2D& cameraRef) : service(ctxRef), camera(cameraRef) {}
};
