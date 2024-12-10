#pragma once

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include <string>

#include "grammar.hpp"

std::vector<int32_t> Grammar::MergeVectors(const std::vector<int32_t>& left, const std::vector<int32_t>& right) {
  std::vector<int32_t> result(left.size() + right.size());
  int32_t index = 0;
  while (index < left.size()) {
    result[index] = left[index];
    ++index;
  }
  while (index < left.size() + right.size()) {
    result[index] = right[index - left.size()];
    ++index;
  }
  return result;
}

bool Grammar::CompareVectors(const std::vector<int32_t>& left, const std::vector<int32_t>& right) {
  int32_t min_len = std::min(left.size(), right.size());
  for (int32_t index = 0; index < min_len; ++index) {
    if (left[index] != right[index]) {
      return left[index] < right[index];
    }
  }
  return left.size() < right.size();
}

int32_t Grammar::GetFreeNonterminal() {
  int32_t ret = free_nonterminal_++;
  return ret;
}

void Grammar::CreateFile(const char* filename) const {
  FILE* tmp_ptr = fopen(filename, "w");
  fclose(tmp_ptr);
  // TODO: delete $filename
}

void Grammar::MarkGenerating(int32_t index, std::vector<bool>& is_generating, std::vector<bool>& used) {
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
          MarkGenerating(pos_[elem], is_generating, used);
        }
        can_generate &= is_generating[pos_[elem]];
      }
    }
    
    if (!found_nonterminal || can_generate) {
      is_generating[index] = true;
    }
  }
}

void Grammar::MarkReachable(int32_t index, std::vector<bool>& used) {
  used[index] = true;
  
  for (const auto& rule : rules_[index]) {
    for (auto elem : rule) {
      if (pos_.count(elem) > 0 && !used[pos_[elem]]) {
        MarkReachable(pos_[elem], used);
      }
    }
  }
}

void Grammar::DeleteNonterminalsWithoutProperty(std::vector<bool>& has_property) {
  std::unordered_map<int32_t, int32_t> new_pos;
  std::vector<std::vector<std::vector<int32_t>>> new_rules;
  std::vector<int32_t> new_nonterminals;
  for (int32_t index = 0; index < nonterminals_.size(); ++index) {
    if (!has_property[index]) {
      continue;          
    }
    new_pos[nonterminals_[index]] = new_nonterminals.size();
    new_nonterminals.push_back(nonterminals_[index]);
    new_rules.push_back({{}});
    new_rules[new_pos[nonterminals_[index]]] = rules_[index];
  }
  pos_ = new_pos;
  rules_ = new_rules;
  nonterminals_ = new_nonterminals;
}

void Grammar::DeleteNongenerating() {
  std::vector<bool> is_generating(nonterminals_.size(), false);
  std::vector<bool> used(nonterminals_.size(), false);
  for (int32_t index = 0; index < nonterminals_.size(); ++index) {
    if (!used[index]) {
      MarkGenerating(index, is_generating, used);
    }
  }
  
  DeleteNonterminalsWithoutProperty(is_generating);
}

void Grammar::DeleteUnreachable() {
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

void Grammar::DeleteMixed() {
  for (auto& list_of_rules : rules_) {
    for (auto& rule : list_of_rules) {
      for (auto& elem : rule) {
        if (pos_.count(elem) == 0) {
          elem = elem + TERMINAL_SHIFT;
        }
      }
    } 
  }
  for (auto elem : terminals_) {
    int new_nonterminal = elem + TERMINAL_SHIFT;
    pos_[new_nonterminal] = nonterminals_.size();
    nonterminals_.push_back(new_nonterminal);
    rules_.push_back({{elem}});
  }
}

void Grammar::DeleteLong() {
  int32_t old_nonterminals_size = nonterminals_.size();
  for (int32_t index = 0; index < old_nonterminals_size; ++index) {
    std::vector<std::pair<int32_t, std::vector<int32_t>>> added_rules;
    std::vector<bool> is_correct(rules_[index].size(), false);
    for (int32_t rule_index = 0; rule_index < rules_[index].size(); ++rule_index) {
      const std::vector<int32_t>& rule = rules_[index][rule_index];
      if (rule.size() < 3) {
        is_correct[rule_index] = true;
        continue;
      }
      
      std::vector<int32_t> curr = MergeVectors({rule[rule.size() - 2]}, {rule[rule.size() - 1]});
      int nonterminal = GetFreeNonterminal();
      added_rules.push_back(std::make_pair(nonterminal, curr));
      
      for (int32_t adding = rule.size() - 3; adding >= 1; --adding) {
        curr[0] = rule[adding];
        curr[1] = nonterminal;
        nonterminal = GetFreeNonterminal();
        added_rules.push_back(std::make_pair(nonterminal, curr));
      }
      curr[0] = rule[0];
      curr[1] = nonterminal;
      nonterminal = nonterminals_[index];
      added_rules.push_back(std::make_pair(nonterminal, curr));
    }
    std::vector<std::vector<int32_t>> new_rules;
    for (int32_t rule_index = 0; rule_index < rules_[index].size(); ++rule_index) {
      if (is_correct[rule_index]) {
        new_rules.push_back(rules_[index][rule_index]);
      }
    }
    
    rules_[index] = new_rules;
    
    for (auto& elem : added_rules) {
      if (pos_.count(elem.first) == 0) {
        pos_[elem.first] = nonterminals_.size();
        nonterminals_.push_back(elem.first);
        rules_.push_back({});
      }
      rules_[pos_[elem.first]].push_back(elem.second);
    }
  }
}

void Grammar::MarkEpsilonGenerative(int32_t index, std::vector<bool>& generates_epsilon, std::vector<bool>& used) {
  used[index] = true;
  for (const auto& rule : rules_[index]) {
    bool epsilon_rule = true;
    for (auto elem : rule) {
      if (pos_.count(elem) == 0) {
        epsilon_rule = false;
        continue;
      }
      if (!used[pos_[elem]]) {
        MarkEpsilonGenerative(pos_[elem], generates_epsilon, used);
      }
      if (!generates_epsilon[pos_[elem]]) {
        epsilon_rule = false;
      }
    }
    generates_epsilon[index] = (epsilon_rule ? true : generates_epsilon[index]);
  }
}

void Grammar::ProcessEpsilonGenerative(std::vector<bool> generates_epsilon) {
  for (int32_t index = 0; index < nonterminals_.size(); ++index) {
    int32_t old_rules_size = rules_[index].size();
    for (int32_t rule_index = 0; rule_index < old_rules_size; ++rule_index) {
      if (rules_[index][rule_index].size() < 2) {
        continue;
      }
      assert(rules_[index][rule_index].size() == 2 && "Wrong length of rule after deleting long rules!\n");
      if (generates_epsilon[pos_[rules_[index][rule_index][0]]]) {
        rules_[index].push_back({rules_[index][rule_index][1]});
      }
      if (generates_epsilon[pos_[rules_[index][rule_index][1]]]) {
        rules_[index].push_back({rules_[index][rule_index][0]});
      }
    }
  }
}

void Grammar::DeleteEpsilonRules() {
  for (int32_t index = 0; index < nonterminals_.size(); ++index) {
    std::vector<std::vector<int32_t>> new_rules;
    for (int32_t rule_index = 0; rule_index < rules_[index].size(); ++rule_index) {
      if (rules_[index][rule_index].empty()) {
        continue;
      }
      new_rules.push_back(rules_[index][rule_index]);
    }
    rules_[index] = new_rules;
  }
}

void Grammar::DeleteEpsilonGenerative() {
  std::vector<bool> generates_epsilon(nonterminals_.size(), false);
  {
    std::vector<bool> used(nonterminals_.size(), false);
    MarkEpsilonGenerative(pos_[START_NONTERMINAL], generates_epsilon, used);
  }
  ProcessEpsilonGenerative(generates_epsilon);
  DeleteEpsilonRules();
  if (generates_epsilon[pos_[START_NONTERMINAL]]) {
    int32_t new_start_nonterminal = GetFreeNonterminal();
    pos_[new_start_nonterminal] = nonterminals_.size();
    rules_.push_back({});
    nonterminals_.push_back(new_start_nonterminal);
    rules_[pos_[new_start_nonterminal]].push_back({START_NONTERMINAL});
    rules_[pos_[new_start_nonterminal]].push_back({});
    START_NONTERMINAL = new_start_nonterminal;
  }
}

bool Grammar::IsSingleRule(const std::vector<int32_t>& rule) {
    if (rule.size() != 1) {
      return false;
    }
    if (pos_.count(rule[0]) == 0) {
      return false;
    }
    return true;
}

void Grammar::TopSort(int32_t index, std::vector<bool>& used, std::vector<int32_t>& order) {
  used[index] = true;
  for (const auto& rule : rules_[index]) {
    if (!IsSingleRule(rule)) {
      continue;
    }
    if (used[pos_[rule[0]]]) {
      continue;
    }
    TopSort(pos_[rule[0]], used, order);
  }
  order.push_back(index);
}

std::vector<std::vector<int32_t>> Grammar::GenerateTransposedGraph() {
  std::vector<std::vector<int32_t>> t_graph(nonterminals_.size());
  for (int32_t index = 0; index < nonterminals_.size(); ++index) {
    for (const auto& rule : rules_[index]) {
      if (!IsSingleRule(rule)) {
        continue;
      }
      t_graph[pos_[rule[0]]].push_back(index);
    }
  }
  return t_graph;
}

void Grammar::WriteComponent(int32_t index, std::vector<int32_t>& color, std::vector<int32_t>& comp, std::vector<std::vector<int32_t>>& graph) {
  comp.push_back(index);
  for (auto to : graph[index]) {
    if (color[to] != -1) {
      continue;
    }
    color[to] = color[index];
    WriteComponent(to, color, comp, graph);
  }
}

std::pair<std::vector<std::vector<int32_t>>, std::vector<int32_t>> Grammar::FindCondensation() {
  std::vector<int32_t> order;
  std::vector<bool> used(nonterminals_.size());
  for (int32_t index = 0; index < nonterminals_.size(); ++index) {
    if (used[index]) {
      continue;
    }
    TopSort(index, used, order); // now we are working with indexes of nonterminals
  }
  reverse(order.begin(), order.end());
  std::vector<std::vector<int32_t>> t_graph = GenerateTransposedGraph();
  std::vector<int32_t> color(nonterminals_.size(), -1);
  std::vector<std::vector<int32_t>> components;
  for (auto index : order) {
    if (color[index] == -1) {
      color[index] = components.size();
      components.push_back({});
      WriteComponent(index, color, components.back(), t_graph);
    }
  }
  return {components, color};
}

std::vector<std::vector<int32_t>> Grammar::FindNonSingleRulesOfComponent(std::vector<int32_t>& comp) {
  std::vector<std::vector<int32_t>> result;
  for (auto index : comp) {
    for (const auto& rule : rules_[index]) {
      if (IsSingleRule(rule)) {
        continue;
      }
      result.push_back(rule);
    }
  }
  sort(result.begin(), result.end(), CompareVectors);
  result.resize(unique(result.begin(), result.end()) - result.begin());
  return result;
}

void Grammar::WriteNewRulesInComponent(std::vector<int32_t>& comp, std::vector<std::vector<int32_t>>& new_rules) {
  for (auto index : comp) {
    rules_[index] = new_rules;
  }
}

void Grammar::WriteTransitiveClosure(int32_t comp_index, std::vector<bool>& used, std::vector<int32_t>& color, std::vector<std::vector<int32_t>>& components) {
  used[comp_index] = true;
  std::vector<int32_t> to;
  for (auto elem : components[comp_index]) {
    for (const auto& rule : rules_[elem]) {
      if (!IsSingleRule(rule)) {
        continue;
      }
      to.push_back(color[pos_[rule[0]]]);
    }
  }
  sort(to.begin(), to.end());
  to.resize(unique(to.begin(), to.end()) - to.begin());
  std::vector<std::vector<int32_t>> new_rules = FindNonSingleRulesOfComponent(components[comp_index]);
  for (auto elem : to) {
    if (!used[elem]) {
      WriteTransitiveClosure(elem, used, color, components);
    }
    for (const auto& rule : rules_[components[elem][0]]) {
      new_rules.push_back(rule);
    }
  }
  WriteNewRulesInComponent(components[comp_index], new_rules);
}

void Grammar::DeleteSingle() {
  std::vector<std::vector<int32_t>> components;
  std::vector<int32_t> color;
  {
    std::pair<std::vector<std::vector<int32_t>>, std::vector<int32_t>> tmp = FindCondensation();
    components = tmp.first;
    color = tmp.second;
  }
  std::vector<bool> used(components.size());
  for (int32_t index = 0; index < components.size(); ++index) {
    if (used[index]) {
      continue;
    }
    WriteTransitiveClosure(index, used, color, components);
  }
}

bool Grammar::InputGrammarFromFile(std::string filename) {
  std::ifstream in(filename.c_str());
  
  int32_t nonterminals_cnt;
  int32_t terminals_cnt;
  int32_t rules_cnt;
  in >> nonterminals_cnt >> terminals_cnt >> rules_cnt;
  
  nonterminals_.resize(nonterminals_cnt);
  terminals_.resize(terminals_cnt);
  rules_.resize(rules_cnt);
  for (int32_t i = 0; i < nonterminals_cnt; ++i) {
    char x;
    in >> x;
    nonterminals_[i] = static_cast<int32_t>(x);
    assert(pos_.count(nonterminals_[i]) == 0 && "Wrong input format!\n");
    pos_[nonterminals_[i]] = i;
  }
  {
    bool found = false;
    for (auto elem : nonterminals_) {
      found |= elem == START_NONTERMINAL;
    }
    assert(found && "No start nonterminal!\n");
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
    tmp = ' ';
    while (tmp == ' ') {
      in.get(tmp);
    }
    std::vector<int32_t> rule;
    while (tmp != '\n') {
      rule.push_back(tmp);
      in.get(tmp);
    }
    rules_[pos_[nonterminal]].push_back(rule);
  }
  char tmp;
  in >> tmp;
  START_NONTERMINAL = static_cast<int32_t>(tmp);
  //std::cerr << "input done" << std::endl;
  return true;
}

void Grammar::NormalizeToKhomsky() {
  DeleteNongenerating();
  DeleteUnreachable();
  
  DeleteMixed();
  DeleteLong();
  PrintGrammar();
  DeleteSingle();
  DeleteEpsilonGenerative();
  
  DeleteNongenerating();
  DeleteUnreachable();
}

void Grammar::PrintGrammar() const {
  for (int32_t index = 0; index < nonterminals_.size(); ++index) {
    std::cout << nonterminals_[index] << ":" << std::endl;
    for (const auto rule : rules_[index]) {
      if (rule.size() == 0) {
        std::cout << "epsilon\n";
        continue;
      }
      if (rule.size() == 1 && pos_.count(rule[0]) == 0) {
        std::cout << static_cast<char>(rule[0]) << '\n';
        continue;
      }
      for (auto elem : rule) {
        std::cout << elem << ' ';
      }
      std::cout << std::endl;
    }
  }
}

void Grammar::SetParser(Parser* parser) { parser_ = parser; }

bool Grammar::ContainsWord(const std::string& word) const {
  return parser_->ContainedInGrammar(word);
}

std::vector<char> Grammar::GetTerminals() const {
  return terminals_;
}

std::vector<int32_t> Grammar::GetNonTerminals() const {
  return nonterminals_;
}

std::vector<std::vector<std::vector<int32_t>>> Grammar::GetRules() const {
  return rules_; 
}

std::unordered_map<int32_t, int32_t> Grammar::GetPos() const {
  return pos_;
}

int32_t Grammar::GetStartNonTerminal() const {
  return START_NONTERMINAL;
}

Parser::Parser(Grammar& g): ptr_g_(&g) {}
