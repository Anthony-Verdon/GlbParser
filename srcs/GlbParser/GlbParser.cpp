#include "GlbParser/GlbParser.hpp"
#include "JsonParser/JsonParser.hpp"
#include "Utils/Utils.hpp"
#include <stdexcept>
#include <iostream>
#include <fstream>

namespace GlbParser
{
    void ParseFile(const std::string &path, bool generateFiles)
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
        std::cout << "Conversion complete!" << std::endl;

        for (auto mesh: gltfJson["meshes"])
        {
            std::cout << mesh["name"] << std::endl;
            for (size_t attribute: mesh["primitives"][0]["attributes"])
            {
                auto accessor = gltfJson["accessors"][attribute];
                size_t bufferViewIndex = accessor["bufferView"];
                size_t count = accessor["count"];
                std::string type = accessor["type"];

                auto bufferView = gltfJson["bufferViews"][bufferViewIndex];
                size_t byteOffset = bufferView["byteOffset"];

                float* buffer = (float*)(binStr.data() + byteOffset);

                if (type == "VEC3")
                {
                    for (size_t i = 0; i < count; ++i)
                        std::cout << "(" << buffer[i * 3] << ", " << buffer[i * 3  + 1] << ", " << buffer[i * 3 + 2] << ")" << std::endl;
                }
                else if (type == "VEC2")
                {
                    for (size_t i = 0; i < count; ++i)
                        std::cout << "(" << buffer[i * 2] << ", " << buffer[i * 2 + 1] << ")" << std::endl;
                }
                else
                {
                    std::cout << "type unknown:" << type << std::endl;
                }
                std::cout << std::endl;
            }
        }
    }
}