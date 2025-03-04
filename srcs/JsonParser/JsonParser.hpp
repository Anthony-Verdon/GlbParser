#pragma once

#include "JsonParser/JsonValue.hpp"
#include <string>

typedef std::string::iterator stringIt;

namespace JsonParser
{
    JsonValue ParseFile(const std::string &path);
    JsonValue ParseJson(const std::string &text, stringIt &it);

    std::pair<std::string, JsonValue> RetrieveKeyValuePair(const std::string& text, stringIt& it); 
    std::string RetrieveKey(const std::string& text, stringIt& it);
    JsonValue RetrieveValue(const std::string& text, stringIt& it);
    JsonValue ParseArray(const std::string &text, stringIt &it);
    JsonValue ParsePrimitive(const std::string &text, stringIt &start, stringIt &end);
    JsonValue ParseString(const std::string &text, stringIt &start, stringIt &end);
    JsonValue ParseKeyword(const std::string &text, stringIt &it);

    void SkipWhitespace(stringIt &it);
    
    std::ostream &operator<<(std::ostream &os, const JsonValue &json);
}