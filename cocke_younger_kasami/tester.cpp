#pragma once

#include <cstdio>

#include "tester.hpp"

Tester::Tester(const std::string& filename) {
    std::ifstream in(filename.c_str());

    std::ofstream grammar_out("grammar_tmp.txt");
    std::ofstream words_out("words_tmp.txt");

    int32_t nonterminals;
    int32_t terminals;
    int32_t rules;
    in >> nonterminals >> terminals >> rules;
    in.get();
    grammar_out << nonterminals << ' ' << terminals << ' ' << rules << '\n';
    for (int32_t cnt = 0; cnt < 2 + rules + 1; ++cnt) {
      std::string tmp;
      getline(in, tmp);
      grammar_out << tmp << '\n';
    }

    int32_t words;
    in >> words;
    in.get();
    words_out << words << '\n';
    for (int32_t cnt = 0; cnt < words; ++cnt) {
      std::string tmp;
      getline(in, tmp);
      words_out << tmp << '\n';
    }

    grammar_out.close();
    words_out.close();
  }

  std::string Tester::RunTest() {
    Grammar g;
    g.InputGrammarFromFile("grammar_tmp.txt");
    CYK parser(g);
    g.SetParser(&parser);

    std::ifstream in_words("words_tmp.txt");
    int32_t queries = 0;
    std::string result;

    in_words >> queries;
    while (queries > 0) {
      std::string word;
      in_words >> word;
      if (g.ContainsWord(word)) {
        result.push_back('1');
      } else {
        result.push_back('0');
      }
      --queries;
    }
    return result;
  }

  Tester::~Tester() {
    remove("grammar_tmp.txt");
    remove("words_tmp.txt");
  }