#include <algorithm>
#include <iostream>
#include <memory>
#include <stack>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>

class jsonString;
class JsonObject;

class jsonObj {
public:
  virtual bool parse(const std::string &s,
                     std::stack<std::unique_ptr<jsonObj>> &) = 0;
  virtual bool parse(std::string::const_iterator &start,
                     std::string::const_iterator end,
                     std::stack<std::unique_ptr<jsonObj>> &) {
    return start == end;
  }

  bool newProcess(std::string::const_iterator &token,
                  std::string::const_iterator end,
                  std::stack<std::unique_ptr<jsonObj>> &data) {
    if (*token == '"') {
      auto &obj = data.emplace(std::make_unique<jsonString>());
      return obj->parse(token, end, data);
    }
    if (*token == '{') {
      auto &obj = data.emplace(std::make_unique<JsonObject>());
      return obj->parse(token, end, data);
    }
    return false;
  }
  bool mParseComplete{false};
};

using DataType = std::stack<std::unique_ptr<jsonObj>>;

class jsonString final : public jsonObj {
public:
public:
  explicit jsonString() { std::cout << "String" << std::endl; }

  bool parse(const std::string &s, std::stack<std::unique_ptr<jsonObj>> &data) {
    return process(s.begin(), s.cend(), data);
  };
  bool parse(std::string::const_iterator &start,
             std::string::const_iterator end,
             std::stack<std::unique_ptr<jsonObj>> &data) {
    return process(start, end, data);
  }

  bool process(std::string::const_iterator token,
               std::string::const_iterator end,
               __attribute__((unused)) DataType &data) {
    while (token != end) {
      switch (mState) {
      case State::Start:
        if (*token != Transition::StartKeyToken) {
          mError = "String not startin by StartToken";
          return false;
        }
        mState = State::Current;
        break;
      case State::Current:
        if (*token == Transition::EndKeyToken) {
          std::cout << "read value: " << mContent << std::endl;
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
  enum Transition : char { StartKeyToken = '"', EndKeyToken = '"' };
  enum State : char { Start, Current };
  std::string mError;
  std::string mContent;

  State mState{Start};
};

class jsonValue final : public jsonObj {
public:
  explicit jsonValue() { std::cout << "Value" << std::endl; }
  bool parse(const std::string &s, DataType &data) override {
    return process(s.begin(), s.cend(), data);
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
          std::cout << "key: " << mKey << std::endl;
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
        return newProcess(token, end, data);
      }
      std::cout << *token << std::endl;
      ++token;
    }
    return true;
  }

  enum Transition : char { StartKeyToken = '"', EndKeyToken = '"' };
  enum State : uint8_t { BeginKey, CurrentKey, Separator, ValueStart };
  State mState{State::BeginKey};
  std::string mKey;
  std::string mError;
};

class JsonObject final : public jsonObj {
public:
  explicit JsonObject() { std::cout << "String" << std::endl; }

  enum Token : char { Start = '{', StringStart = '\"', End = '}' };
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
          auto &obj = data.emplace(std::make_unique<jsonValue>());
          mState = State::CurrentValue;
          return obj->parse(token, end, data);
        } else if (*token == Token::End) {
          mParseComplete = true;
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
      ++token;
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
      if (obj->mParseComplete == true) {
        mData.pop();
      }
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
