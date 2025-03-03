#pragma once

#include <map>
#include <string>
#include <variant>

typedef std::string::iterator stringIt;

namespace JsonParser
{
    struct JsonValue;
    typedef std::map<std::string, JsonValue> JsonMap;

    struct JsonValue : std::variant<int, double, JsonMap> {
        using variant::variant;
    };

    JsonValue ParseFile(const std::string &path);
    JsonValue ParseJson(const std::string &text, stringIt &it);
    std::pair<std::string, JsonValue> RetriveKeyValuePair(const std::string& text, stringIt& it); 
    JsonValue ParsePrimitive(const std::string &text, stringIt &start, stringIt &end);

    std::ostream &operator<<(std::ostream &os, const JsonValue &json);
}