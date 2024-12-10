#include "tester.hpp"
#include "test_cyk.hpp"

TEST(PalindromeTestCase, IsNotPalindrome) {
  ASSERT_EQ("00000", Tester("testing/testing_cyk/tests/is_not_palindrome.txt").RunTest());
}

TEST(PalindromeTestCase, IsPalindrome) {
  ASSERT_EQ("10110101", Tester("testing/testing_cyk/tests/is_palindrome.txt").RunTest());
}

TEST(LongRulesTestCase, LongRules) {
  ASSERT_EQ("110001", Tester("testing/testing_cyk/tests/long_rules.txt").RunTest());
}

TEST(TransitiveClosureTestCase, TransitiveClosure) {
  ASSERT_EQ("111101010", Tester("testing/testing_cyk/tests/transitive_closure.txt").RunTest());
}

TEST(EpsilonRulesTestCase, EpsilonRules) {
  ASSERT_EQ("11110001", Tester("testing/testing_cyk/tests/epsilon_rules.txt").RunTest());
}

TEST(DefaultTestCase, Default) {
  ASSERT_EQ("10", Tester("testing/testing_cyk/tests/default.txt").RunTest());
}

