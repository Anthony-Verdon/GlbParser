#include "JsonParser/JsonValue.hpp"
#include <stdexcept>

namespace JsonParser
{
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
}