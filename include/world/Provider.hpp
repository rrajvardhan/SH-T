#pragma once

#include "Camera2D.hpp"
#include "EventBus.hpp"
#include "ServiceContext.hpp"

struct WorldProvider
{
  ServiceContext& service;
  Camera2D&       camera;
  EventBus&       eventbus;

  WorldProvider(ServiceContext& ctxRef, Camera2D& cameraRef, EventBus& eventbusRef)
      : service(ctxRef), camera(cameraRef), eventbus(eventbusRef)
  {
  }
};
