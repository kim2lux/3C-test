#include <algorithm>
#include <iostream>
#include <memory>
#include <stack>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>

class jsonObj {
public:
  virtual bool parse(const std::string &s,
                     std::stack<std::unique_ptr<jsonObj>> &) = 0;
  virtual bool parse(std::string::const_iterator &start, std::string::const_iterator end) {
      return start == end;
  }
  bool doneParsing{false};
};

using DataType = std::stack<std::unique_ptr<jsonObj>>;

class jsonString {
public:
  enum transition : char { start = '"', end = '"' };
};

class jsonValue final : public jsonObj {
  bool parse(const std::string &s, DataType &mData) override{
      return process(s.begin(), s.cend());
  }

  bool parse(std::string::const_iterator &start, std::string::const_iterator end) override {
      return process(start, end);
  }

  bool process(std::string::const_iterator token, std::string::const_iterator end) {
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
                    std::cout << "found key: " << mKey << std::endl;
                    mState = State::Separator;
                 } else {
                    mKey.append(1, *token);
                 }
                break;
                case State::Separator:
                if (*token != ':') {
                    mError = std::string{"Should get a separator"};
                }
                break;
          }
          std::cout << *token << std::endl;
          ++token;
      }
      return true;
  }

  enum Transition : char { StartKeyToken = '"', EndKeyToken = '"'};
  enum State : uint8_t { BeginKey, CurrentKey, Separator, Value };
  State mState{State::BeginKey};
  std::string mKey;
  std::string mError;
};

class JsonObject final : public jsonObj {
public:
  enum Token : char { Start = '{', StringStart = '\"', End = '}' };
  bool parse(const std::string &s, DataType &mData) override {
    for (auto token = s.cbegin(); token != s.cend(); ++token) {
      switch (mState) {
      case State::Begin:
        if (*token != Token::Start) {
          mError = std::string{"Object not starting"};
        }
        mState = State::StartValue;
        break;
      case State::StartValue:
        if (*token == Token::StringStart) {
          auto& obj = mData.emplace(std::make_unique<jsonValue>());
          return obj->parse(token, s.cend());
          mState = State::CurrentValue;
        } else if (*token == Token::End) {
          doneParsing = true;
        } else {
          mError = std::string{"No StartVale or End token available"};
          return false;
        }
        break;
      case State::CurrentValue: {
        ;
      } break;
      }
      std::cout << *token << std::endl;
    }
    return true;
  }
  enum State : uint8_t { Begin, StartValue, CurrentValue };

  State mState{State::Begin};
  std::string mError;
};

class JsonParser {
public:
  void compute(std::string &line) {
    line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
    if (newObj) {
      getObj(line);
      newObj = false;
    } else {
      auto &obj = mData.top();
      obj->parse(line, mData);
    }
  }

  bool newObj{true};

private:
  void getObj(const std::string &s) {
    if (*s.cbegin() == JsonObject::Token::Start) {
      auto &obj = mData.emplace(std::make_unique<JsonObject>());
      obj->parse(s, mData);
    }
  }
  DataType mData;
};

int main() {
  JsonParser json;

  for (std::string line; std::getline(std::cin, line);) {
    json.compute(line);
  }
}
