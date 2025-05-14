#pragma once

#include <string>

#include "cyk.hpp"


class Tester{
 public:
  Tester(const std::string& filename);

  std::string RunTest();

  ~Tester();
};
