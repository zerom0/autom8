/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gtest/gtest.h"

#include "Resources/Formula.h"

#include <list>
#include <string>
#include <vector>

double eval(std::list<std::string> formula, std::vector<std::string> variables);

TEST(Formula, empty) {
  std::list<std::string> formula;
  std::vector<std::string> variables;
  ASSERT_EQ(0, eval(formula, variables));
}

TEST(Formula, single_number) {
  std::list<std::string> formula = {"23.4711"};
  std::vector<std::string> variables;
  ASSERT_EQ(23.4711, eval(formula, variables));
}

TEST(Formula, two_number) {
  std::list<std::string> formula = {"23.4711", "4711"};
  std::vector<std::string> variables;
  ASSERT_EQ(4711, eval(formula, variables));
}

TEST(Formula, addition) {
  std::list<std::string> formula = {"23.4711", "1", "+"};
  std::vector<std::string> variables;
  ASSERT_EQ(24.4711, eval(formula, variables));
}

TEST(Formula, substraction) {
  std::list<std::string> formula = {"23.4711", "1", "-"};
  std::vector<std::string> variables;
  ASSERT_EQ(22.4711, eval(formula, variables));
}

TEST(Formula, multiplication) {
  std::list<std::string> formula = {"23.4711", "2", "*"};
  std::vector<std::string> variables;
  ASSERT_EQ(46.9422, eval(formula, variables));
}

TEST(Formula, division) {
  std::list<std::string> formula = {"23.4711", "2", "/"};
  std::vector<std::string> variables;
  ASSERT_NEAR(11.7355, eval(formula, variables), 0.0001);
}

TEST(Formula, one_variable) {
  std::list<std::string> formula = {"$1"};
  std::vector<std::string> variables = {"1.23"};
  ASSERT_EQ(1.23, eval(formula, variables));
}

TEST(Formula, divide_two_variables) {
  std::list<std::string> formula = {"$1", "$2", "/"};
  std::vector<std::string> variables = {"1", "2"};
  ASSERT_EQ(0.5, eval(formula, variables));
}

TEST(Formula, undefined_variable) {
  std::list<std::string> formula = {"$1"};
  std::vector<std::string> variables;
  ASSERT_EQ(0, eval(formula, variables));
}

