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
  virtual bool parse(const std::string &s) = 0;
};

class jsonString {
public:
  enum transition : char { start = '"', end = '"' };
};

class jsonValue {
  enum transition : char { start = '"', end = '"' };
};

class JsonObject final : public jsonObj {
public:
  enum Token : char { Start = '{', End = '}' };
  bool parse(const std::string &s) override {
    for (auto token = s.cbegin(); token != s.cend(); ++token) {
        std::cout << *token << std::endl;
    }
    return true;
  }
  enum State : uint8_t { Begin, Value, Stop };

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
      obj->parse(line);
    }
  }

  bool newObj{true};

private:
  void getObj(const std::string &s) {
    if (*s.cbegin() == JsonObject::Token::Start) {
      auto &obj = mData.emplace(std::make_unique<JsonObject>());
      obj->parse(s);
    }
  }
  std::stack<std::unique_ptr<jsonObj>> mData;
};

int main() {
  JsonParser json;

  for (std::string line; std::getline(std::cin, line);) {
    json.compute(line);
  }
}
