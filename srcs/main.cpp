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

        return (EXIT_SUCCESS);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (EXIT_FAILURE);
    }
}