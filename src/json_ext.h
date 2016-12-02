/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <CoAP/json.h>

namespace CoAP {
/**
 * Initializes a map with the key from the JSON string.
 */
template<typename T>
void from_json(const std::string& json, std::map<std::string, T>& result) {

  struct Helper {
    void operator()(Helper& next, const std::string& json, std::map<std::string, T>& result) {
      auto commaPos = CoAP::__internal__::findListDelimiter(json);
      auto element = json.substr(0, commaPos);
      auto colonPos = element.find_first_of(':');
      if (colonPos != std::string::npos) {
        std::string k;
        CoAP::from_json(CoAP::__internal__::trimmed(element.substr(0, colonPos)), k);
        T v;
        CoAP::from_json(CoAP::__internal__::trimmed(element.substr(colonPos + 1)), v);
        result.emplace(k, v);
      }
      if (commaPos != std::string::npos) next(next, json.substr(commaPos + 1), result);
    }
  } helper;

  if (json.size()) helper(helper, CoAP::__internal__::withoutCurly(json), result);
}
} // namespace CoAP
