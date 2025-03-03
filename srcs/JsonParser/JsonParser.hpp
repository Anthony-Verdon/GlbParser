#pragma once

#include <map>
#include <string>
#include <variant>
#include <vector>
#include <iterator> 
#include <cstddef> 

typedef std::string::iterator stringIt;

namespace JsonParser
{
    struct JsonValue;
    typedef std::map<std::string, JsonValue> JsonMap;
    typedef std::vector<JsonValue> JsonArray;

    struct JsonValue : std::variant<int, double, std::string, bool, void*, JsonArray, JsonMap> 
    {
        using variant::variant;
        JsonValue &operator[](const char *key) { return std::get<JsonMap>(*this)[key]; }
        JsonValue &operator[](int index) { return std::get<JsonArray>(*this)[index]; }
        operator int() const { return std::get<int>(*this); }
        operator double() const { return std::get<double>(*this); }
        operator std::string() const { return std::get<std::string>(*this); }
        operator bool() const { return std::get<bool>(*this); }
        operator void*() const { return NULL; }
        operator JsonArray() const { return std::get<JsonArray>(*this); }
        operator JsonMap() const { return std::get<JsonMap>(*this); }

        struct Iterator
        {
            using ArrayIt = JsonArray::iterator;
            using MapIt = JsonMap::iterator;
            using VariantIt = variant<ArrayIt, MapIt>;

            Iterator(const ArrayIt &it) {this->it = it; }
            Iterator(const MapIt &it) {this->it = it; }

            const JsonValue &operator*() 
            {
                if (const ArrayIt *ptr = std::get_if<ArrayIt>(&it)) { return (*(*ptr)); }
                if (const MapIt *ptr = std::get_if<MapIt>(&it)) { return (*(*ptr)).second; }
                throw(0);
            }
            JsonValue *operator->()
            {
                if (const ArrayIt *ptr = std::get_if<ArrayIt>(&it)) { return &(**ptr); }
                if (const MapIt *ptr = std::get_if<MapIt>(&it)) { return &((**ptr).second); }
                 return (NULL); 
            }

            // Prefix increment
            Iterator& operator++()
            {
                if (ArrayIt *ptr = std::get_if<ArrayIt>(&it)) { (*ptr)++; }
                if (MapIt *ptr = std::get_if<MapIt>(&it)) { (*ptr)++; }
                return *this; 
            }  
            
            // Postfix increment
            Iterator operator++(int) 
            { 
                Iterator tmp = *this; 
                ++(*this); 
                return tmp; 
            }
            
            friend bool operator== (const Iterator& a, const Iterator& b) { return a.it == b.it; };
            friend bool operator!= (const Iterator& a, const Iterator& b) { return a.it != b.it; };
            
            private:
                VariantIt it;
        };

        Iterator begin();
        Iterator end();
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