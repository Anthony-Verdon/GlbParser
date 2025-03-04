#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace Utils
{
  bool isInt(const std::string &word);
  bool isFloat(const std::string &words);
  std::vector<std::string> splitLine(std::string line, const std::string &delimiter);
  float DegToRad(float angle);
  std::string readFile(const std::string &path, std::ios_base::openmode mode = std::ios_base::in);
  bool checkExtension(const std::string &path, const std::string &extension);
}
  