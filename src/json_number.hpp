#pragma once

#include "type.hpp"

class JsonNumber final : public JsonObj {
public:
public:
  explicit JsonNumber(bool all=false) : mAll{all} {}

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
        if (*token == ',') {
          for (size_t i = 0; i < data.size(); ++i) {
            std::cout << " ";
          }
          std::cout << "Number: " << std::stoi(mContent.c_str()) << std::endl;
          mParseComplete = true;
          return true;
        }
        if (mAll) {
            ;
        }
        else if (*token < Transition::Lower || Transition::Upper < *token) {
          mParseComplete = true;
          mError = std::string{"Number parsing error"};
          return false;
        }
      }
      mContent.append(1, *token);
      ++token;
    }
    if (!mAll) {
    std::cout << "Number: " << std::stoi(mContent.c_str()) << std::endl;

    }
    mParseComplete = true;
    return true;
  }

private:
  enum Transition : char { Lower = '0', Upper = '9' };
  enum State : char { Start };
  std::string mContent;

  State mState{Start};
  bool mAll{false};
};