#pragma once

#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include <string>

class Grammar {
  private:
    std::vector<int32_t> nonterminals_;
    std::vector<char> terminals_;
    std::vector<std::vector<std::string>> rules_; // rules_[i] = {rules for nonterminals_[i]}
    
    std::unordered_map<int32_t, int32_t> pos_;

    const int32_t START_NONTERMINAL = 'S';
    const char* TMP_FILE_NAME = "input_tmp.txt";
    const int32_t TERMINAL_SHIFT = 1'000;

    void CreateFile(const char* filename) const {
      FILE* tmp_ptr = fopen(filename, "w");
      fclose(tmp_ptr);
      // TODO: delete $filename
    }

    void MarkGenerating(int32_t index, std::vector<bool>& is_generating, std::vector<bool>& used) {
      used[index] = true;
      
      for (const auto& rule : rules_[index]) {
        if (rule.empty()) { // epsilon
          is_generating[index] = true;
        }
        
        bool found_nonterminal = false;
        bool can_generate = true;
        
        for (auto elem : rule) {
          if (pos_.count(elem) > 0) {
            found_nonterminal = true;
            if (!used[pos_[elem]]) {
              MarkGenerating(used[pos_[elem]], is_generating, used);
            }
            can_generate &= is_generating[pos_[elem]];
          }
        }
        
        if (!found_nonterminal || can_generate) {
          is_generating[index] = true;
        }
      }
    }

    void MarkReachable(int32_t index, std::vector<bool>& used) {
      used[index] = true;
      
      for (const auto& rule : rules_[index]) {
        for (auto elem : rule) {
          if (pos_.count(elem) > 0 && !used[pos_[elem]]) {
            MarkReachable(pos_[elem], used);
          }
        }
      }
    }

    void DeleteNonterminalsWithoutProperty(std::vector<bool>& has_property) {
      std::unordered_map<int32_t, int32_t> new_pos;
      std::vector<std::vector<std::string>> new_rules;
      std::vector<int32_t> new_nonterminals;

      for (int32_t index = 0; index < nonterminals_.size(); ++index) {
        if (!has_property[index]) {
          continue;          
        }
        new_pos[nonterminals_[index]] = new_nonterminals.size();
        new_nonterminals.push_back(nonterminals_[index]);
        new_rules.push_back({""});
        new_rules[new_pos[nonterminals_[index]]] = rules_[index];
      }

      pos_ = new_pos;
      rules_ = new_rules;
      nonterminals_ = new_nonterminals;
    }

    void DeleteNongenerating() {
      std::vector<bool> is_generating(nonterminals_.size(), false);
      std::vector<bool> used(nonterminals_.size(), false);

      for (int32_t index = 0; index < nonterminals_.size(); ++index) {
        if (!used[index]) {
          MarkGenerating(index, is_generating, used);
        }
      }
      
      DeleteNonterminalsWithoutProperty(is_generating);
    }

    void DeleteUnreachable() {
      int32_t start = -1;
      
      for (int32_t index = 0; index < nonterminals_.size() && start == -1; ++index) {
        if (nonterminals_[index] == START_NONTERMINAL) {
          start = index;
        }
      }
      assert(start != -1 && "Start nonterminal not found!\n");
      
      std::vector<bool> used(nonterminals_.size());
      MarkReachable(start, used);
      
      DeleteNonterminalsWithoutProperty(used);
    }

    void DeleteMixed() {
      
    }

  public:
    bool InputGrammarFromStdin() {
      CreateFile(TMP_FILE_NAME);

      std::ofstream out(TMP_FILE_NAME);
      std::string input;
      
      while (getline(std::cin, input)) {
        out << input;
      }
      
      out.close();
      return InputGrammarFromFile(TMP_FILE_NAME);
    }

    bool InputGrammarFromFile(std::string filename = "") {
      std::ifstream in(filename.c_str());
      
      int32_t nonterminals_cnt;
      int32_t terminals_cnt;
      int32_t rules_cnt;
      in >> nonterminals_cnt >> terminals_cnt >> rules_cnt;
      
      nonterminals_.resize(nonterminals_cnt);
      terminals_.resize(terminals_cnt);
      rules_.resize(rules_cnt);

      for (int32_t i = 0; i < nonterminals_cnt; ++i) {
        in >> nonterminals_[i];
        assert(pos_.count(nonterminals_[i]) == 0 && "Wrong input format!\n");
        pos_[nonterminals_[i]] = i;
      }
      {
        bool found = false;
        for (auto elem : nonterminals_) {
          found |= elem == START_NONTERMINAL;
        }
        assert(found && "No start state!\n");
      }
      for (int32_t i = 0; i < terminals_cnt; ++i) {
        in >> terminals_[i];
      }
      for (int32_t i = 0; i < rules_cnt; ++i) {
        char nonterminal;
        in >> nonterminal;
        bool found = false;
        for (auto elem : nonterminals_) {
          found |= elem == nonterminal;
        }
        assert(found && "Wrong rule format!\n");
        char tmp;
        in >> tmp;
        assert(tmp == '-' && "Wrong rule format!\n");
        in >> tmp;
        in.get(tmp);
        std::string rule = "";
        while (tmp != '\n') {
          rule.push_back(tmp);
          in.get(tmp);
        }
        rules_[pos_[nonterminal]].push_back(rule);
      }
      return true;
    }

    void KhomskyNormalizer() {
      DeleteNongenerating();
      DeleteUnreachable();
      DeleteMixed();
    }
};