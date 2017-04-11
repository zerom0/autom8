/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Formula.h"

#include "Resource.h"

#include <coap/json.h>
#include <coap/Logging.h>
#include <vector>

using std::placeholders::_1;
using std::placeholders::_2;

SETLOGLEVEL(LLDEBUG)


double eval(std::list<std::string> formula, std::vector<std::string> variables) {
  std::list<std::string> stack;
  while (!formula.empty()) {
    std::string token = formula.front();
    formula.pop_front();

    if (token == "+") {
      auto r = std::stod(stack.back()); stack.pop_back();
      auto l = std::stod(stack.back()); stack.pop_back();
      stack.emplace_back(std::to_string(l + r));
    } else if (token == "-") {
      auto r = std::stod(stack.back()); stack.pop_back();
      auto l = std::stod(stack.back()); stack.pop_back();
      stack.emplace_back(std::to_string(l - r));
    } else if (token == "*") {
      auto r = std::stod(stack.back()); stack.pop_back();
      auto l = std::stod(stack.back()); stack.pop_back();
      stack.emplace_back(std::to_string(l * r));
    } else if (token == "/") {
      auto r = std::stod(stack.back()); stack.pop_back();
      auto l = std::stod(stack.back()); stack.pop_back();
      stack.emplace_back(std::to_string(l / r));
    } else if (token[0] == '$') {
      int i = std::stoi(&token[1]) - 1;
      if (i >= 0 && i < variables.size())
        stack.emplace_back(variables[i]);
    } else {
      double value = std::stod(token);
      stack.emplace_back(token);
    }
  }

  if (stack.empty()) stack.emplace_back("0");
  double value = std::stod(stack.back());
  return value;
}

/*
 * The Formula transforms one value into another value based on the given formula in RPN
 *
 * inputURI   rw   Path to the resource that will provide the input value
 * inputValue r    Last value (double) received from the observed resource
 * formula    rw   RPN formula als list of tokens like "2 $1 *"
 * value      r    The output value
 */

Resource* newFormulaResource(InputValueUpdated callback, const std::map<std::string, std::string>& values) {
  TLOG << "newFormulaResource()\n";

  auto r = new Resource();
  r->createProperty("value", Property::ReadOnly, Property::Volatile);

  r->createProperty("formula", Property::ReadWrite, Property::Persistent);
  r->getProperty("formula")->setValue("$1", true);

  auto value = r->createProperty("inputValue", std::bind(callback, r, "inputValue", _1, _2), Property::Volatile);
  r->createProperty("inputURI", std::bind(inputURIUpdated, value, _1, _2), Property::Persistent);

  r->init(values);

  return r;
}

void formulaResourceUpdated(Resource* resource, const std::string& propertyName, const std::string& oldValue, const std::string& newValue) {
  DLOG << "formulaResourceUpdated(..., " << propertyName << ", " << oldValue << ", " << newValue << ")\n";

  if (propertyName != "inputValue") return;

  try {
    std::string formulaString = resource->getProperty("formula")->getValue();
    std::stringstream ss(formulaString);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::list<std::string> formula(begin, end);

    std::vector<std::string> variables = {newValue};
    resource->getProperty("value")->setValue(CoAP::to_json(eval(formula, variables)), true);
  }
  catch (std::runtime_error& e) {

  }
}

std::unique_ptr<Resource> formulaResourceFactory(const std::map<std::string, std::string>& values) {
  return std::unique_ptr<Resource>(newFormulaResource(formulaResourceUpdated, values));
}
