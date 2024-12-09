#include "cyk.cpp"

int main() {
  Grammar g;
  g.InputGrammarFromFile("../input.txt");
  CYK parser(g);
  g.SetParser(&parser);
  std::string word;
  std::cin >> word;
  if (g.ContainsWord(word)) {
    std::cout << "Yes\n";
  } else {
    std::cout << "No\n";
  }
}