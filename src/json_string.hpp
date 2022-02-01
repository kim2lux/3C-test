#pragma once

#include "type.hpp"

class JsonString final : public JsonObj {
public:
public:
  explicit JsonString() {}

  bool parse(const std::string &s, std::stack<std::unique_ptr<JsonObj>> &data) {
    auto start = s.begin();
    return process(start, s.cend(), data);
  };
  bool parse(std::string::const_iterator &start,
             std::string::const_iterator end,
             std::stack<std::unique_ptr<JsonObj>> &data) {
    return process(start, end, data);
  }

  bool process(std::string::const_iterator &token,
               std::string::const_iterator end,
               __attribute__((unused)) DataType &data) {
    while (token != end) {
      switch (mState) {
      case State::Start:
        if (*token != Transition::StartKeyToken) {
          mError = "String not starting by StartToken";
          return false;
        }
        mState = State::Current;
        break;
      case State::Current:
        if (*token == Transition::EndKeyToken) {
          for (size_t i = 0; i < data.size(); ++i) {
            std::cout << " ";
          }
          std::cout << "value: " << mContent << std::endl;
          mParseComplete = true;
          return true;
        }
        mContent.append(1, *token);
        break;
      }
      ++token;
    }
    return true;
  }

private:
  enum Transition : char { StartKeyToken = '"', EndKeyToken = '"' };
  enum State : char { Start, Current };
  std::string mContent;

  State mState{Start};
};