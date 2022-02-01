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

#include "type.hpp"
#include "json_string.hpp"
#include "json_array.hpp"
#include "json_number.hpp"
#include "json_value.hpp"
#include "json_object.hpp"

class JsonParser {
public:
  void compute(std::string &line) {
    line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
    if (newObj) {
      getObj(line);
      newObj = false;
    } else {
      auto &obj = mData.top();
      if (obj->parse(line, mData) == false) {
        std::cout << "Error Parsing: " << line << std::endl;
      }

      while (mData.size() > 0) {
        auto &todelete = mData.top();
        if (todelete->mError.size() > 0) {
          std::cout << "Error: " << todelete->mError << std::endl;
        }
        if (todelete->mParseComplete == true) {
          mData.pop();
        } else {
          return;
        }
      }
    }
  }

  bool newObj{true};

private:
  void getObj(const std::string &s) {
    if (*s.cbegin() == JsonObject::Token::Start) {
      auto &obj = mData.emplace(std::make_unique<JsonObject>());
      if (obj->parse(s, mData) == false) {
        std::cout << "Warning... fail parsing..." << std::endl;
      }
    }
  }
  DataType mData;
};