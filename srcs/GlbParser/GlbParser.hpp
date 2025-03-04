#pragma once

#include <string>
#include "JsonParser/JsonValue.hpp"

namespace GlbParser
{
    std::pair<JsonParser::JsonValue, std::string> ParseFile(const std::string &path, bool generateFiles = false);
}