#include "GlbParser/GlbParser.hpp"
#include "JsonParser/JsonParser.hpp"
#include "Utils/Utils.hpp"
#include <stdexcept>
#include <iostream>
#include <fstream>

namespace GlbParser
{
    std::pair<JsonParser::JsonValue, std::string> ParseFile(const std::string &path, bool generateFiles)
    {
        if (!Utils::checkExtension(path, ".glb"))
            throw(std::runtime_error("wrong extension, only parse .glb file"));

        std::string data = Utils::readFile(path, std::ios::binary);

        if (data.size() < 12)            
            throw(std::runtime_error("Invalid GLB file!"));

        // Read GLB Header
        uint32_t jsonLength = *reinterpret_cast<uint32_t*>(&data[12]);
        std::string jsonStr(data.begin() + 20, data.begin() + 20 + jsonLength);
        
        // Parse JSON
        stringIt it = jsonStr.begin();
        JsonParser::JsonValue gltfJson = JsonParser::ParseJson(jsonStr, it);
        
        // Extract binary buffer
        uint32_t binOffset = 20 + jsonLength;
        if (binOffset >= data.size())
            throw(std::runtime_error("Invalid GLB file!"));
        std::string binStr(data.begin() + binOffset + 8, data.end());
        
        // Generate Files
        if (generateFiles)
        {
            std::string filename = path.substr(0, path.find_last_of("."));
            std::ofstream binFile(filename + ".bin", std::ios::binary);
            binFile << binStr;

            gltfJson["buffers"][0]["bin"] = filename.substr(filename.find_last_of("/") + 1, filename.size()) + ".bin";
            std::ofstream gltfFile(filename + ".gltf");
            gltfFile << gltfJson;
        }

        return (std::make_pair(gltfJson, binStr));
    }
}