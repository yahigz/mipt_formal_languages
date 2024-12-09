

#include "grammar.cpp"

class CYK : public Parser {
 private:
  std::vector<std::vector<int32_t>> produced_by_; // index of nonterminal -> {indexes of terminals}

  std::vector<char> terminals_; // order of terminals here may differ from order in grammar
  std::vector<int32_t> nonterminals_;
  std::vector<std::vector<std::vector<int32_t>>> rules_;
  int32_t START_NONTERMINAL_;
  bool contains_epsilon_ = false;

  std::unordered_map<int32_t, int32_t> nonterminals_pos_;
  std::unordered_map<char, int32_t> terminals_pos_;

  void WriteProducedBy();

  void DoPrecalc() override;

 public:

  bool ContainedInGrammar(const std::string& word) override;
};
