#include "Utils.hpp"
#include <cmath>
#include <fstream>
std::vector<std::string> Utils::splitLine(std::string line, const std::string &delimiter)
{
    size_t index;
    std::string word;
    std::vector<std::string> words;

    index = 0;
    while (index != std::string::npos)
    {
        index = line.find(delimiter);
        word = line.substr(0, index);
        if (word.length() > 0)
            words.push_back(word);
        line = line.substr(index + 1, std::string::npos);
    }
    return words;
}

float Utils::DegToRad(float angle)
{
    return (roundf(angle * (M_PI / 180) * 100000) / 100000);
}

std::stringstream Utils::readFile(const std::string &path)
{
    std::ifstream file;
    std::stringstream stream;

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    file.open(path);
    stream << file.rdbuf();
    file.close();
    return (stream);
}

bool Utils::checkExtension(const std::string &path, const std::string &extension)
{
    std::string filename = path.substr(path.find_last_of("/") + 1, path.length());
    if (extension.size() >= filename.size())
        return (false);
    for (size_t i = 0; i < extension.size(); i++)
    {
        if (filename[filename.size() - extension.size() + i] != extension[i])
            return (false);
    }
    return (true);
}

bool Utils::isInt(const std::string &word)
{
    const std::string numbers = "0123456789";
    size_t start = 0;
    if (word[0] == '-')
        start++;

    for (size_t i = start; i < word.size(); i++)
    {
        if (numbers.find(word[i]) == std::string::npos)
            return (false);
    }
    return (true);
}

bool Utils::isFloat(const std::string &word)
{
    const std::string numbers = "0123456789";
    size_t start = 0;
    bool pointFound = false;
    if (word[0] == '-')
        start++;

    for (size_t i = start; i < word.size(); i++)
    {
        if (word[i] == '.')
        {
            if (pointFound || i == 0 || (word[0] == '-' && i == 1))
                return (false);
            else
            {
                pointFound = true;
                continue;
            }
        }
        if (numbers.find(word[i]) == std::string::npos)
            return (false);
    }
    return (true);
}