#include "JsonParser/JsonParser.hpp"
#include "Utils/Utils.hpp"
#include <cassert>
#include <cctype> 

namespace JsonParser
{
    JsonValue ParseFile(const std::string &path)
    {
        std::stringstream file = Utils::readFile(path);
        std::string text = file.str();

        stringIt start = text.begin();
        return (ParseJson(text, start));
    }

    JsonValue ParseJson(const std::string &text, stringIt &it)
    {
        assert(*it == '{');
        it++;

        std::map<std::string, JsonValue> json;
        while (*it != '}')
        {
            const auto [key, value] = RetriveKeyValuePair(text, it);
            json[key] = value;

            while (*it == ' ' || *it == '\n')
                it++;
        }
        it++;

        return json;
    }

    std::pair<std::string, JsonValue> RetriveKeyValuePair(const std::string& text, stringIt& it)
    {
        assert(it != text.end());

        while (*it == ' ' || *it == '\n')
            it++;

        stringIt currentIt;
        std::string key;
        JsonValue value;

        if (*it == '\"')
        {
            currentIt = ++it;
            while (*it != '\"')
                it++;
        }

        key = text.substr(currentIt - text.begin(), it - currentIt);
        assert(*(++it) == ':');
        it++;

        while (*it == ' ' || *it == '\n')
            it++;

        if (*it == '{')
        {
            value = ParseJson(text, it);
        }
        else
        {
            currentIt = it;
            while (isdigit(*it) || *it == '.')
                it++;
            value = ParsePrimitive(text, currentIt, it);
        }

        if (*it == ',')
            it++;

        return (std::make_pair(key, value));
    }

    JsonValue ParsePrimitive(const std::string &text, stringIt &start, stringIt &end)
    {
        std::string substr = text.substr(start - text.begin(), end - start);
        int pointIndex = substr.find(".");

        if (pointIndex >= (end - start)) // integer
            return std::stoi(substr);
        else                             // float(double)
            return std::stod(substr);
    }

    std::ostream &operator<<(std::ostream &os, const JsonValue &json)
    {
        static size_t level = 0;
        if (const int *ptr = std::get_if<int>(&json))
            os << *ptr << ", ";
        else if (const double *ptr = std::get_if<double>(&json))
            os << *ptr << ", ";
        else if (const JsonMap *ptr = std::get_if<JsonMap>(&json))
        {
            os << "{" << std::endl;
            for (size_t i = 0; i < level; i++)
                os << "\t";

            level++;
            for (auto it = ptr->begin(); it != ptr->end(); it++)
            {
                os << it->first << ": " << it->second << std::endl;
            }
            level--;    
            for (size_t i = 0; i < level - 1; i++)
                os << "\t";
            os << "}" << std::endl;
        }
        return (os);
    }
}