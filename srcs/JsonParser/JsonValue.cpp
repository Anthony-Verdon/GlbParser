#include "JsonParser/JsonValue.hpp"
#include <stdexcept>
#include <iostream>

namespace JsonParser
{
    JsonValue::operator float() const
    {
        if (const int *ptr = std::get_if<int>(this)) 
            return static_cast<float>(*ptr);
        if (const double *ptr = std::get_if<double>(this))
            return static_cast<float>(*ptr);
        return (0);
    }

    const JsonValue &JsonValue::Iterator::operator*() 
    {
        if (const ArrayIt *ptr = std::get_if<ArrayIt>(&it)) 
            return (*(*ptr));
        if (const MapIt *ptr = std::get_if<MapIt>(&it))
            return (*(*ptr)).second;
        throw(std::runtime_error("JsonValue::Iterator::operator*, invalid value"));
    }

    JsonValue *JsonValue::Iterator::operator->()
    {
        if (const ArrayIt *ptr = std::get_if<ArrayIt>(&it))
            return &(**ptr);
        if (const MapIt *ptr = std::get_if<MapIt>(&it))
            return &((**ptr).second);
        return (NULL); 
    }

    JsonValue::Iterator& JsonValue::Iterator::operator++()
    {
        if (ArrayIt *ptr = std::get_if<ArrayIt>(&it))
            (*ptr)++;
        if (MapIt *ptr = std::get_if<MapIt>(&it)) 
            (*ptr)++;
        return *this; 
    }  
    
    JsonValue::Iterator JsonValue::Iterator::operator++(int) 
    { 
        Iterator tmp = *this; 
        ++(*this); 
        return tmp; 
    }

    std::string JsonValue::Iterator::key() const
    {
        if (const MapIt *ptr = std::get_if<MapIt>(&it))
            return ((*ptr)->first);
        else
            throw(std::runtime_error("JsonValue::Iterator::key(), invalid value"));
    }

    JsonValue JsonValue::Iterator::value() const
    {
        if (const MapIt *ptr = std::get_if<MapIt>(&it))
            return ((*ptr)->second);
        else
            throw(std::runtime_error("JsonValue::value(), invalid value"));
    }

    JsonValue::Iterator JsonValue::begin()
    {
        if (JsonArray *ptr = std::get_if<JsonArray>(this))
            return (Iterator(ptr->begin()));
        else if (JsonMap *ptr = std::get_if<JsonMap>(this))
            return (Iterator(ptr->begin()));
        else
            throw(std::runtime_error("JsonValue::begin(), invalid value"));
    }

    JsonValue::Iterator JsonValue::end()
    {
        if (JsonArray *ptr = std::get_if<JsonArray>(this))
            return (Iterator(ptr->end()));
        else if (JsonMap *ptr = std::get_if<JsonMap>(this))
            return (Iterator(ptr->end()));
        else
        throw(std::runtime_error("JsonValue::end(), invalid value"));
    }

    bool JsonValue::KeyExist(const std::string &key) const
    {
        if (const JsonMap *ptr = std::get_if<JsonMap>(this))
            return (ptr->find(key) != ptr->end());

        return (false);
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