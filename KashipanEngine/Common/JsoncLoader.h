#pragma once
#include <json.hpp>

namespace KashipanEngine {

using Json = nlohmann::json;

Json LoadJsonc(const std::string &filepath);

} // namespace KashipanEngine