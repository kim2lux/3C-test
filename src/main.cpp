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
class JsonObject;
class JsonArray;

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
    }
    else if (*token == '{') {
      auto &obj = data.emplace(std::make_unique<JsonObject>());
      return obj->parse(token, end, data);
    }
    else if (*token == '[') {
      auto &obj = data.emplace(std::make_unique<JsonArray>());
      return obj->parse(token, end, data);
    }
    return false;
  }
  bool mParseComplete{false};
  std::string mError;
};

using DataType = std::stack<std::unique_ptr<JsonObj>>;

class JsonString final : public JsonObj {
public:
public:
  explicit JsonString() { std::cout << "String" << std::endl; }

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
          mError = "String not startin by StartToken";
          return false;
        }
        mState = State::Current;
        break;
      case State::Current:
        if (*token == Transition::EndKeyToken) {
          std::cout << "Json String: " << mContent << std::endl;
          if (mContent == std::string{"CreateNewDoc()"}) {
              std::cout << "debug" << std::endl;
          }
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

class JsonValue final : public JsonObj {
public:
  explicit JsonValue() { std::cout << "Value" << std::endl; }
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
          std::cout << "key: " << mKey << std::endl;
          if (mKey == std::string{"onclick"}) {
              std::cout << "dbg" << std::endl;
          }
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
        newProcess(token, end, data);
        mState = State::ValueEnd;
        break;
      case State::ValueEnd:
        if (*token != Terminate) {
          mError = std::string{"Not terminated"};
          return false;
        }
        mParseComplete = true;
        break;
      }
      std::cout << *token << std::endl;
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

class JsonArray final : public JsonObj {
public:
  explicit JsonArray() { std::cout << "Array" << std::endl; }
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
          std::cout << "Array Done" << std::endl;
          mParseComplete = true;
          return true;
        }
        else {
            return (newProcess(token, end, data));
            mState = State::Separator;
        }
        break;
      case State::Separator:
         if (*token == Transition::NewElem) {
             mState = State::NewElement;
         }
         else {
             mError = std::string{"Error parsing N array elements"};
             return false;
         }
      }
      std::cout << *token << std::endl;
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

class JsonObject final : public JsonObj {
public:
  enum Token : char { Start = '{', StringStart = '\"', End = '}' };

  explicit JsonObject() { std::cout << "JsonObject" << std::endl; }

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
          mParseComplete = true;
        } else {
          mError = std::string{"No StartVale or End token available"};
          return false;
        }
        break;
      }
      std::cout << *token << std::endl;
      ++token;
    }
    return true;
  }

private:
  enum State : uint8_t { Begin, StartValue, CurrentValue };

  State mState{State::Begin};
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
      if (obj->parse(line, mData) == false) {
        std::cout << "Error Parsing: " << line << std::endl;
      }

      while (mData.size() > 0) {
        auto &todelete = mData.top();
        if (todelete->mError.size() > 0) {
          std::cout << "Error: " << todelete->mError << std::endl;
        }
        if (todelete->mParseComplete == true) {
          std::cout << "done parse: " << std::endl;
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
        std::cout << "ERROR: " << obj->mError << std::endl;
        exit(0);
      }
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
