#pragma once

#include "Event.hpp"

class HandlerFunctionBase
{
public:
  virtual void exec(Event* evnt) = 0;
  virtual ~HandlerFunctionBase() = default;
};

template <typename T, typename EventType>
class MemberFunctionHandler : public HandlerFunctionBase
{
public:
  using MemberFunc = void (T::*)(EventType*);

  MemberFunctionHandler(T* instance, MemberFunc fn) : _instance(instance), _fn(fn) {}

  void exec(Event* evnt) override { (_instance->*_fn)(static_cast<EventType*>(evnt)); }

private:
  T*         _instance;
  MemberFunc _fn;
};
