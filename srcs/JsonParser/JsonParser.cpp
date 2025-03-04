#include "JsonParser/JsonParser.hpp"
#include "Utils/Utils.hpp"
#include <cassert>
#include <cctype> 
#include <iostream>

namespace JsonParser
{
    JsonValue ParseFile(const std::string &path)
    {
        if (!Utils::checkExtension(path, ".json"))
            std::cerr << path << " has wrong extension" << std::endl;

        std::string text = Utils::readFile(path);

        stringIt start = text.begin();
        return (ParseJson(text, start));
    }

    JsonValue ParseJson(const std::string &text, stringIt &it)
    {
        assert(it != text.end());
        assert(*it == '{');
        it++;

        JsonMap json;
        while (*it != '}')
        {
            const auto [key, value] = RetrieveKeyValuePair(text, it);
            json[key] = value;

            SkipWhitespace(text, it);
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
        SkipWhitespace(text, it);
        assert(it != text.end());

        stringIt currentIt;

        if (*it == '\"')
        {
            currentIt = ++it;
            while (it != text.end() && *it != '\"')
                it++;
        }

        assert(it != text.end());
        assert(*it == '\"');
        std::string key = text.substr(currentIt - text.begin(), it - currentIt);
        it++;
        return (key);
    }

    JsonValue RetrieveValue(const std::string& text, stringIt& it)
    {
        SkipWhitespace(text, it);
        assert(it != text.end());
        if (*it == ':')
            it++;
        SkipWhitespace(text, it);
        assert(it != text.end());
        
        stringIt currentIt;
        JsonValue value;
        if (*it == '{')
        {
            value = ParseJson(text, it);
        }
        else if (*it == '[')
        {
            it++;
            value = ParseArray(text, it);
        }
        else if (*it == '-' || isdigit(*it) || *it == 'e' || *it == 'E')
        {
            currentIt = it;
            while (it != text.end() && (*it == '-' || isdigit(*it) || *it == '.' || *it == 'e' || *it == 'E'))
                it++;
            value = ParsePrimitive(text, currentIt, it);
        }
        else if (*it == '"')
        {
            currentIt = ++it;
            while (it != text.end() && *it != '"')
                it++;
            value = ParseString(text, currentIt, it);
            it++;
        }
        else
        {
            value = ParseKeyword(text, it);
        }

        assert(it != text.end());
        if (*it == ',')
            it++;

        return (value);
    }
    
    JsonValue ParseArray(const std::string &text, stringIt &it)
    {
        JsonArray values;

        while (it != text.end() && *it != ']')
        {
            const auto value = RetrieveValue(text, it);
            values.push_back(value);

            SkipWhitespace(text, it);
        }
        assert(it != text.end());
        it++;

        return (values);
    }

    JsonValue ParsePrimitive(const std::string &text, stringIt &start, stringIt &end)
    {
        assert(start != text.end() && end != text.end());
        std::string substr = text.substr(start - text.begin(), end - start);
        int pointIndex = substr.find(".");

        if (pointIndex == -1) // integer
            return std::stoi(substr);
        else                  // float(double)
            return std::stod(substr);
    }

    JsonValue ParseString(const std::string &text, stringIt &start, stringIt &end)
    {
        assert(start != text.end() && end != text.end());
        return (text.substr(start - text.begin(), end - start));
    }

    JsonValue ParseKeyword(const std::string &text, stringIt &it)
    {
        assert(it != text.end());
        stringIt currentIt = it;
        for (int i = 0; i < 4; i++)
        {
            it++;
            assert(it != text.end());
        }

        if (text.substr(currentIt - text.begin(), it - currentIt) == "true")
            return (true);
        else if (text.substr(currentIt - text.begin(), it - currentIt) == "null")
            return (nullptr);
        else
        {
            it++;
            assert(it != text.end());
            if (text.substr(currentIt - text.begin(), it - currentIt) == "false")
                return (false);
            else
                return (nullptr);
        }
    }

    void SkipWhitespace(const std::string &text, stringIt &it)
    {
        static const std::string whitespace = "\t\n\v\f\r ";
        while (it != text.end() && whitespace.find(*it) != std::string::npos)
            it++;
        assert(it != text.end());
    }

    std::ostream &operator<<(std::ostream &os, const JsonValue &json)
    {
        static size_t level = 0;
        if (const int *ptr = std::get_if<int>(&json))
            os << *ptr;
        else if (const double *ptr = std::get_if<double>(&json))
            os << *ptr;
        else if (const std::string *ptr = std::get_if<std::string>(&json))
            os << '"' << *ptr << '"';
        else if (const bool *ptr = std::get_if<bool>(&json))
            os << (*ptr ? "true" : "false");
        else if (const void *ptr = std::get_if<void*>(&json))
            os << "NULL";
        else if (const JsonArray *ptr = std::get_if<JsonArray>(&json))
        {
            os << "[";
            level++;
            for (size_t i = 0; i < ptr->size(); )
            {
                os << '\n';
                for (size_t i = 0; i < level; i++)
                    os << '\t';
                os << (*ptr)[i];
                i++;
                if (i < ptr->size())
                    os << ", ";
            }
            level--;
            os << '\n';
            for (size_t i = 0; i < level; i++)
                    os << '\t';
            os << "]";
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
                os << '"' << it->first << '"' << ": " << it->second;
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