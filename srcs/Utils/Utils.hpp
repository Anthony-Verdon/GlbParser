#pragma once

#include <sstream>
#include <string>
#include <vector>

class Utils
{
  public:
    static bool isInt(const std::string &word);
    static bool isFloat(const std::string &words);
    static std::vector<std::string> splitLine(std::string line, const std::string &delimiter);
    static float DegToRad(float angle);
    static std::stringstream readFile(const std::string &path);
    static bool checkExtension(const std::string &path, const std::string &extension);
};
