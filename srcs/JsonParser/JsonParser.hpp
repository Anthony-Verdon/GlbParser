#pragma once

#include <map>
#include <string>
#include <variant>

typedef std::string::iterator stringIt;

namespace JsonParser
{
    struct JsonValue;
    typedef std::map<std::string, JsonValue> JsonMap;

    struct JsonValue : std::variant<int, double, std::string, bool, void*, JsonMap> {
        using variant::variant;
        JsonValue &operator[](const char *key) { return std::get<JsonMap>(*this)[key]; }
        operator int() const { return std::get<int>(*this); }
    };

    JsonValue ParseFile(const std::string &path);
    JsonValue ParseJson(const std::string &text, stringIt &it);

    std::pair<std::string, JsonValue> RetrieveKeyValuePair(const std::string& text, stringIt& it); 
    JsonValue ParsePrimitive(const std::string &text, stringIt &start, stringIt &end);
    JsonValue ParseString(const std::string &text, stringIt &start, stringIt &end);
    JsonValue ParseKeyword(const std::string &text, stringIt &it);

    std::ostream &operator<<(std::ostream &os, const JsonValue &json);
}