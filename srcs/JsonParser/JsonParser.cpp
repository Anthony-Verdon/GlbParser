#include "JsonParser/JsonParser.hpp"
#include "Utils/Utils.hpp"
#include <cassert>
#include <cctype> 
#include <iostream>

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

        JsonMap json;
        while (*it != '}')
        {
            const auto [key, value] = RetrieveKeyValuePair(text, it);
            json[key] = value;

            while (*it == ' ' || *it == '\n')
                it++;
        }
        it++;

        return json;
    }

    std::pair<std::string, JsonValue> RetrieveKeyValuePair(const std::string& text, stringIt& it)
    {
        return (std::make_pair(RetrieveKey(text, it), RetrieveValue(text, it)));
    }

    std::string RetrieveKey(const std::string& text, stringIt& it)
    {
        while (*it == ' ' || *it == '\n')
            it++;

        stringIt currentIt;

        if (*it == '\"')
        {
            currentIt = ++it;
            while (*it != '\"')
            {
                it++;
            }
        }
        std::string key = text.substr(currentIt - text.begin(), it - currentIt);
        it++;
        return (key);
    }

    JsonValue RetrieveValue(const std::string& text, stringIt& it)
    {
        stringIt currentIt;
        JsonValue value;

        if (*it == ':')
            it++;

        while (*it == ' ' || *it == '\n')
            it++;

        if (*it == '{')
        {
            value = ParseJson(text, it);
        }
        else if (*it == '[')
        {
            it++;
            value = ParseArray(text, it);
        }
        else if (isdigit(*it))
        {
            currentIt = it;
            while (isdigit(*it) || *it == '.')
                it++;
            value = ParsePrimitive(text, currentIt, it);
        }
        else if (*it == '"')
        {
            currentIt = it++;
            while (*it != '"')
                it++;
            it++;
            value = ParseString(text, currentIt, it);
        }
        else
        {
            value = ParseKeyword(text, it);
        }

        if (*it == ',')
            it++;

        return (value);
    }
    
    JsonValue ParseArray(const std::string &text, stringIt &it)
    {
        std::vector<JsonValue> values;

        while (*it != ']')
        {
            const auto value = RetrieveValue(text, it);
            values.push_back(value);

            while (*it == ' ' || *it == '\n')
                it++;
        }
        it++;

        return (values);
    }

    JsonValue ParsePrimitive(const std::string &text, stringIt &start, stringIt &end)
    {
        std::string substr = text.substr(start - text.begin(), end - start);
        int pointIndex = substr.find(".");

        if (pointIndex == -1) // integer
            return std::stoi(substr);
        else                  // float(double)
            return std::stod(substr);
    }

    JsonValue ParseString(const std::string &text, stringIt &start, stringIt &end)
    {
        return (text.substr(start - text.begin(), end - start));
    }

    JsonValue ParseKeyword(const std::string &text, stringIt &it)
    {
        stringIt currentIt = it;
        it = it + 4;

        if (text.substr(currentIt - text.begin(), it - currentIt) == "true")
            return (true);
        else if (text.substr(currentIt - text.begin(), it - currentIt) == "null")
            return (nullptr);
        else
        {
            it++;
            if (text.substr(currentIt - text.begin(), it - currentIt) == "false")
                return (false);
            else
                return (nullptr);
        }
    }

    std::ostream &operator<<(std::ostream &os, const JsonValue &json)
    {
        static size_t level = 0;
        if (const int *ptr = std::get_if<int>(&json))
            os << *ptr;
        else if (const double *ptr = std::get_if<double>(&json))
            os << *ptr;
        else if (const std::string *ptr = std::get_if<std::string>(&json))
            os << *ptr;
        else if (const bool *ptr = std::get_if<bool>(&json))
            os << (*ptr ? "true" : "false");
        else if (const void *ptr = std::get_if<void*>(&json))
            os << "NULL";
        else if (const std::vector<JsonValue> *ptr = std::get_if<std::vector<JsonValue>>(&json))
        {
            os << "[ ";
            for (size_t i = 0; i < ptr->size(); )
            {
                os << (*ptr)[i];
                i++;
                if (i < ptr->size())
                    os << ", ";
            }
            os << " ]";
        }
        else if (const JsonMap *ptr = std::get_if<JsonMap>(&json))
        {
            os << '{';
            level++;
            for (auto it = ptr->begin(); it != ptr->end(); )
            {
                os << '\n';
                for (size_t i = 0; i < level; i++)
                    os << '\t';
                os << it->first << ": " << it->second;
                it++;
                if (it != ptr->end())
                 os << ',';
            }
            level--;
            os << '\n';
            for (size_t i = 0; i < level; i++)
                    os << '\t';
            os << '}';
        }
        return (os);
    }
}