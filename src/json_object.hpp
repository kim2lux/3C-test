#pragma once

#include "type.hpp"

class JsonObject final : public JsonObj {
public:
  enum Token : char { Start = '{', StringStart = '\"', End = '}' };

  explicit JsonObject() {}

  bool parse(std::string::const_iterator &start,
             std::string::const_iterator end, DataType &data) override {
    return process(start, end, data);
  }

  bool parse(const std::string &s, DataType &data) override {
    return process(s.begin(), s.cend(), data);
  }

  bool process(std::string::const_iterator token,
               std::string::const_iterator end, DataType &data) {
    while (token != end) {
      switch (mState) {
      case State::Begin:
        if (*token != Token::Start) {
          mError = std::string{"Object not starting"};
        }
        mState = State::StartValue;
        break;
      case State::StartValue:
        if (*token == Token::StringStart) {
          auto &obj = data.emplace(std::make_unique<JsonValue>());
          mState = State::StartValue;
          return obj->parse(token, end, data);
        } else if (*token == Token::End) {
          for (size_t i = 0; i < data.size(); ++i) {
            std::cout << " ";
          }
          std::cout << "Json Object close" << std::endl;
          mParseComplete = true;
        } else if (mParseComplete && *token == ',') {
          mParseComplete = true;
        } else {
          mError = std::string{"No StartVale or End token available"};
          return false;
        }
        break;
      }
      ++token;
    }
    return true;
  }

private:
  enum State : uint8_t { Begin, StartValue };

  State mState{State::Begin};
};

