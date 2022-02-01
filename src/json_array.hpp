#pragma once

#include "type.hpp"


class JsonArray final : public JsonObj {
public:
  explicit JsonArray(size_t size) {
    for (size_t i = 0; i < size; ++i) {
      std::cout << " ";
    }
    std::cout << "Create JsonArray" << std::endl;
  }
  bool parse(const std::string &s, DataType &data) override {
    auto start = s.begin();
    return process(start, s.cend(), data);
  }

  bool parse(std::string::const_iterator &start,
             std::string::const_iterator end, DataType &data) override {
    return process(start, end, data);
  }

  bool process(std::string::const_iterator token,
               std::string::const_iterator end, DataType &data) {
    while (token != end) {
      switch (mState) {
      case State::BeginKey:
        mState = State::NewElement;
        if (*token != Transition::StartKeyToken) {
          mError = std::string{"String should start in new value"};
          return false;
        }
        break;
      case State::NewElement:
        if (*token == EndKeyToken) {
          for (size_t i = 0; i < data.size(); ++i) {
            std::cout << " ";
          }

          std::cout << "Array Close" << std::endl;
          mParseComplete = true;
          return true;
        } else {
          return (newProcess(token, end, data));
          mState = State::Separator;
        }
        break;
      case State::Separator:
        if (*token == Transition::NewElem) {
          mState = State::NewElement;
        } else {
          mError = std::string{"Error parsing N array elements"};
          return false;
        }
      }
      ++token;
    }
    return true;
  }

private:
  enum Transition : char {
    StartKeyToken = '[',
    EndKeyToken = ']',
    NewElem = ','
  };

  enum State : uint8_t {
    BeginKey,
    NewElement,
    Separator,
  };
  State mState{State::BeginKey};
};