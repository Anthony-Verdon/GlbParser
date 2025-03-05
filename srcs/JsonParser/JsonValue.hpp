#pragma once

#include <map>
#include <string>
#include <vector>
#include <variant>

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
        operator size_t() const { return static_cast<size_t>(std::get<int>(*this)); }
        operator double() const { return std::get<double>(*this); }
        operator float() const;
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

            const JsonValue &operator*();
            JsonValue *operator->();
            Iterator& operator++(); // Prefix increment
            Iterator operator++(int); // Postfix increment
            
            friend bool operator== (const Iterator& a, const Iterator& b) { return a.it == b.it; };
            friend bool operator!= (const Iterator& a, const Iterator& b) { return a.it != b.it; };
            
            std::string key() const;
            JsonValue value() const;

            private:
                VariantIt it;
        };

        Iterator begin();
        Iterator end();

    };
    
    std::ostream &operator<<(std::ostream &os, const JsonValue &json);
}