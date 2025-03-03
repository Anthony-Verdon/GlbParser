#include "GlbParser/GlbParser.hpp"
#include "JsonParser/JsonParser.hpp"
#include <stdexcept>
#include <iostream>

int main(int argc, char **argv)
{
    try
    {
        if (argc != 2)
            throw(std::runtime_error("one argument needed"));

        GlbParser::ParseFile(argv[1]);

        /*
        JsonParser::JsonValue json = JsonParser::ParseFile(argv[1]);
        std::cout << json << std::endl;

        std::string s1 = json["array"][0];
        std::cout << s1 << std::endl;
        
        int a = json["two"]["three"];
        std::cout << a << std::endl;
        
        std::string s = json["string"];
        std::cout << s << std::endl;
        
        void* ptr = json["void"];
        std::cout << ptr << std::endl;
        
        bool b = json["bool"];
        std::cout << b << std::endl;
        
        double d = json["one"];
        std::cout << d << std::endl;

        std::vector<JsonParser::JsonValue> v = json["array"];
        for (size_t i = 0; i < v.size(); i++)
            std::cout << v[i] << std::endl;

        std::cout << "---" << std::endl;
        for (auto it: json["array"])
            std::cout << it << std::endl;

        std::cout << "---" << std::endl;
        for (auto it: json["read"])
            std::cout << it["salut"] << std::endl;
        */
        return (EXIT_SUCCESS);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (EXIT_FAILURE);
    }
}