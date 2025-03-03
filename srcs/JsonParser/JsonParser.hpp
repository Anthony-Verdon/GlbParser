#pragma once

#include <map>
#include <string>
#include <variant>
#include <vector>

typedef std::string::iterator stringIt;

namespace JsonParser
{
    struct JsonValue;
    typedef std::map<std::string, JsonValue> JsonMap;

    struct JsonValue : std::variant<int, double, std::string, bool, void*, std::vector<JsonValue>, JsonMap> {
        using variant::variant;
        JsonValue &operator[](const char *key) { return std::get<JsonMap>(*this)[key]; }
        JsonValue &operator[](int index) { return std::get<std::vector<JsonValue>>(*this)[index]; }
        operator int() const { return std::get<int>(*this); }
        operator double() const { return std::get<double>(*this); }
        operator std::string() const { return std::get<std::string>(*this); }
        operator bool() const { return std::get<bool>(*this); }
        operator void*() const { return NULL; }
        operator std::vector<JsonValue>() const { return std::get<std::vector<JsonValue>>(*this); }
        operator JsonMap() const { return std::get<JsonMap>(*this); }
    };

    JsonValue ParseFile(const std::string &path);
    JsonValue ParseJson(const std::string &text, stringIt &it);

    std::pair<std::string, JsonValue> RetrieveKeyValuePair(const std::string& text, stringIt& it); 
    std::string RetrieveKey(const std::string& text, stringIt& it);
    JsonValue RetrieveValue(const std::string& text, stringIt& it);
    JsonValue ParseArray(const std::string &text, stringIt &it);
    JsonValue ParsePrimitive(const std::string &text, stringIt &start, stringIt &end);
    JsonValue ParseString(const std::string &text, stringIt &start, stringIt &end);
    JsonValue ParseKeyword(const std::string &text, stringIt &it);

    std::ostream &operator<<(std::ostream &os, const JsonValue &json);
}