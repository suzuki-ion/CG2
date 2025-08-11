#include <fstream>
#include <sstream>

#include "JsoncLoader.h"

namespace KashipanEngine {

Json LoadJsonc(const std::string &filename) {
    Json jsonData;
    std::fstream jsonFile(filename);
    jsonData = Json::parse(jsonFile, nullptr, false, true);
    return jsonData;
}

} // namespace KashipanEngine