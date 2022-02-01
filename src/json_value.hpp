#pragma once

#include "type.hpp"

class JsonValue final : public JsonObj {
public:
  explicit JsonValue() {}
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
        mState = State::CurrentKey;
        if (*token != Transition::StartKeyToken) {
          mError = std::string{"String should start in new value"};
          return false;
        }
        break;
      case State::CurrentKey:
        if (*token == EndKeyToken) {
          for (size_t i = 0; i < data.size(); ++i) {
            std::cout << " ";
          }
          std::cout << "Key: " << mKey << std::endl;
          //   if (mKey == std::string{"onclick"}) {
          //       std::cout << "dbg" << std::endl;
          //   }
          mState = State::Separator;
        } else {
          mKey.append(1, *token);
        }
        break;
      case State::Separator:
        if (*token != ':') {
          mError = std::string{"Should get a separator"};
        }
        mState = State::ValueStart;
        break;
      case State::ValueStart:
        mState = State::ValueEnd;
        if (newProcess(token, end, data) == true) {
          mParseComplete = true;
          return true;
        }
        break;
      case State::ValueEnd:
        if (*token != Terminate) {
          mError = std::string{"Not terminated"};
          return false;
        }
        mParseComplete = true;
        break;
      }
      ++token;
    }
    return true;
  }

private:
  enum Transition : char {
    StartKeyToken = '"',
    EndKeyToken = '"',
    Terminate = ','
  };
  enum State : uint8_t {
    BeginKey,
    CurrentKey,
    Separator,
    ValueStart,
    ValueEnd
  };
  State mState{State::BeginKey};
  std::string mKey;
};