#pragma once

#include "cyk.cpp"


class Tester{
 public:
  Tester(const std::string& filename);

  std::string RunTest();

  ~Tester();
};