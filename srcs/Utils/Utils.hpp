#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

namespace Utils
{
    std::string readFile(const std::string &path, std::ios_base::openmode mode = std::ios_base::in )
    {
        std::ifstream file;
        std::stringstream stream;

        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        file.open(path, mode);
        stream << file.rdbuf();
        file.close();
        return (stream.str());
    }

    bool checkExtension(const std::string &path, const std::string &extension)
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

    float DegToRad(float angle)
    {
        return (roundf(angle * (M_PI / 180) * 100000) / 100000);
    }
}
  