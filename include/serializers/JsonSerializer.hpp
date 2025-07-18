#pragma once

#include "Json.hpp"
#include <stack>
#include <string>

class JSONSerializer
{
public:
  bool saveToFile(const std::string& filename) const
  {
    std::ofstream file(filename);
    if (!file)
      return false;
    file << _root.dump(2);
    return true;
  }

  JSONSerializer& StartNewObject(const std::string& key = "")
  {
    _contextStack.push(JSON::object());
    _keyStack.push(key);
    return *this;
  }

  JSONSerializer& EndObject()
  {
    auto obj = _contextStack.top();
    _contextStack.pop();
    auto key = _keyStack.top();
    _keyStack.pop();

    if (_contextStack.empty())
    {
      if (key.empty())
        _root = obj;
      else
        _root[key] = obj;
    }
    else
    {
      auto& parent = _contextStack.top();
      parent[key]  = obj;
    }

    return *this;
  }

  template <typename T>
  JSONSerializer& AddKeyValuePair(const std::string& key, const T& value)
  {
    if (_contextStack.empty())
      _root[key] = value;
    else
      _contextStack.top()[key] = value;
    return *this;
  }

  const JSON& get() const { return _root; }

private:
  JSON                    _root;
  std::stack<JSON>        _contextStack;
  std::stack<std::string> _keyStack;
};
