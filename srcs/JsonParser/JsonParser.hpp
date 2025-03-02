#pragma once

#include <map>
#include <string>

typedef std::string::iterator stringIt;

namespace JsonParser
{
    union JsonValue
    {
        int i;
        double d;
        std::map<std::string, JsonValue> *json;
    };

    JsonValue ParseJson(const std::string &path);
    JsonValue ParseJson(const std::string &text, stringIt &it);
    std::pair<std::string, JsonValue> RetriveKeyValuePair(const std::string& text, stringIt& it); 
    JsonValue ParsePrimitive(const std::string &text, stringIt &start, stringIt &end);
}