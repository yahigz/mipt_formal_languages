#pragma once

#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

class Parser;

class Grammar {
  protected:
    std::vector<int32_t> nonterminals_;
    std::vector<char> terminals_;
    std::vector<std::vector<std::vector<int32_t>>> rules_; // rules_[i] = {rules for nonterminals_[i]}
    std::unordered_map<int32_t, int32_t> pos_;

  private:
    Parser* parser_;

    static std::vector<int32_t> MergeVectors(const std::vector<int32_t>& left, const std::vector<int32_t>& right);
    static bool CompareVectors(const std::vector<int32_t>& left, const std::vector<int32_t>& right);
    
    int32_t START_NONTERMINAL = 'S';
    const char* TMP_FILE_NAME = "input_tmp.txt"; // using in input from stdin
    const int32_t TERMINAL_SHIFT = 1'000; // using in deleting mixed rules
    int32_t free_nonterminal_ = 2'000; // using in deleting long rules

    // assistive
    bool IsSingleRule(const std::vector<int32_t>& rule);
    int32_t GetFreeNonterminal();
    void CreateFile(const char* filename) const;
    void DeleteNonterminalsWithoutProperty(std::vector<bool>& has_property);

    // deleting nongenerating
    void WriteWhere(std::vector<std::vector<std::pair<int32_t, int32_t>>>& where);
    void WriteCount(std::vector<std::vector<int32_t>>& count, const std::vector<std::vector<std::pair<int32_t, int32_t>>>& where);
    void WriteQueue(std::queue<std::pair<int32_t, int32_t>>& q, const std::vector<std::vector<int32_t>>& count);

    void DeleteNongenerating();

    // deleting unreachable
    void MarkReachable(int32_t index, std::vector<bool>& used);

    void DeleteUnreachable();

    void DeleteMixed();

    void DeleteLong();

    // deleting epsilon generative
    void MarkEpsilonGenerative(int32_t index, std::vector<bool>& generates_epsilon, std::vector<bool>& used);
    void ProcessEpsilonGenerative(std::vector<bool> generates_epsilon);
    void DeleteEpsilonRules();
  
    void DeleteEpsilonGenerative();
    
    // condensation
    void TopSort(int32_t index, std::vector<bool>& used, std::vector<int32_t>& order);
    std::vector<std::vector<int32_t>> GenerateTransposedGraph();
    std::pair<std::vector<std::vector<int32_t>>, std::vector<int32_t>> FindCondensation();

    // deleting single
    std::vector<std::vector<int32_t>> FindNonSingleRulesOfComponent(std::vector<int32_t>& comp);
    void WriteComponent(int32_t index, std::vector<int32_t>& color, std::vector<int32_t>& comp, std::vector<std::vector<int32_t>>& graph);
    void WriteNewRulesInComponent(std::vector<int32_t>& comp, std::vector<std::vector<int32_t>>& new_rules);
    void WriteTransitiveClosure(int32_t comp_index, std::vector<bool>& used, std::vector<int32_t>& color, std::vector<std::vector<int32_t>>& components);

    void DeleteSingle();

  public:

    bool InputGrammarFromFile(std::string filename);
    void PrintGrammar() const;

    void NormalizeToKhomsky();

    friend class Parser;

    void SetParser(Parser* parser);

    std::vector<char> GetTerminals() const;
    std::vector<int32_t> GetNonTerminals() const;
    std::vector<std::vector<std::vector<int32_t>>> GetRules() const;
    std::unordered_map<int32_t, int32_t> GetPos() const;
    int32_t GetStartNonTerminal() const;

    bool ContainsWord(const std::string& word) const;
};


class Parser {
 protected:
  Grammar* ptr_g_;

  void virtual DoPrecalc() = 0;

 public:
  Parser(Grammar& g);

  bool virtual ContainedInGrammar(const std::string& word) = 0;
};
