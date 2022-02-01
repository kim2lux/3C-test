#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <stack>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>

class JsonString;
class JsonNumber;
class JsonObject;
class JsonArray;
class JsonValue;

class JsonObj {
public:
  virtual bool parse(const std::string &s,
                     std::stack<std::unique_ptr<JsonObj>> &) = 0;
  virtual bool parse(std::string::const_iterator &start,
                     std::string::const_iterator end,
                     std::stack<std::unique_ptr<JsonObj>> &) {
    return start == end;
  }

  bool newProcess(std::string::const_iterator &token,
                  std::string::const_iterator end,
                  std::stack<std::unique_ptr<JsonObj>> &data) {
    if (*token == '"') {
      auto &obj = data.emplace(std::make_unique<JsonString>());
      return obj->parse(token, end, data);
    } else if (*token >= '0' && *token <= '9') {
      auto &obj = data.emplace(std::make_unique<JsonNumber>());
      return obj->parse(token, end, data);
    } else if (*token == 't' || *token == 'f') {
      auto &obj = data.emplace(std::make_unique<JsonNumber>(true));
      return obj->parse(token, end, data);
    } else if (*token == '{') {
      auto &obj = data.emplace(std::make_unique<JsonObject>());
      return obj->parse(token, end, data);
    } else if (*token == '[') {
      auto &obj = data.emplace(std::make_unique<JsonArray>(data.size()));
      return obj->parse(token, end, data);
    }
    return false;
  }
  bool mParseComplete{false};
  std::string mError;
};
using DataType = std::stack<std::unique_ptr<JsonObj>>;
