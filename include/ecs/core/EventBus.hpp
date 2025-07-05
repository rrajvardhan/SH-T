#pragma once

#include "HandlerFunctionBase.hpp"
#include <list>
#include <map>
#include <typeindex>

class EventBus
{
public:
  template <typename EventType>
  void publish(EventType* event)
  {
    auto it = _subscribers.find(typeid(EventType));
    if (it == _subscribers.end())
      return;

    for (auto& handler : *it->second)
    {
      handler->exec(event);
    }

    delete event;
  }

  template <typename T, typename EventType>
  void subscribe(T* instance, void (T::*memberFn)(EventType*))
  {
    auto& handlers = _subscribers[typeid(EventType)];
    if (!handlers)
      handlers = new std::list<HandlerFunctionBase*>();
    handlers->push_back(new MemberFunctionHandler<T, EventType>(instance, memberFn));
  }

  ~EventBus()
  {
    for (auto& [_, handlerList] : _subscribers)
    {
      for (auto* handler : *handlerList)
        delete handler;
      delete handlerList;
    }
  }

private:
  std::map<std::type_index, std::list<HandlerFunctionBase*>*> _subscribers;
};
