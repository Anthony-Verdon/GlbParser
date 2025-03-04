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
  std::stringstream readFile(const std::string &path);
  std::vector<char> readBinaryFile(const std::string& path); 
  bool checkExtension(const std::string &path, const std::string &extension);
}
  