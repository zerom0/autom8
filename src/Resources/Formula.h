/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <map>
#include <memory>

class Resource;

/**
 * Factory for a new resource of type History.
 *
 * @return Ownership of heap allocated resource.
 */
std::unique_ptr<Resource> formulaResourceFactory(const std::map<std::string, std::string>& values);
