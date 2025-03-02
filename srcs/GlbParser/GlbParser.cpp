#include "GlbParser/GlbParser.hpp"
#include "Utils/Utils.hpp"
#include <stdexcept>
#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

// Function to read a binary file
std::vector<char> readBinaryFile(const std::string& filename) 
{
    std::ifstream file(filename, std::ios::binary);
    return std::vector<char>(std::istreambuf_iterator<char>(file), {});
}

namespace GlbParser
{
    void ParseFile(const std::string &path)
    {
        if (!Utils::checkExtension(path, ".glb"))
            throw(std::runtime_error("wrong extension, only parse .glb file"));

        std::vector<char> data = readBinaryFile(path);

        if (data.size() < 12) {
            std::cerr << "Invalid GLB file!" << std::endl;
            return;
        }

        // Read GLB Header
        uint32_t jsonLength = *reinterpret_cast<uint32_t*>(&data[12]);
        std::string jsonStr(data.begin() + 20, data.begin() + 20 + jsonLength);

        // Parse JSON
        json gltfJson = json::parse(jsonStr);

        // Extract binary buffer
        size_t binOffset = 20 + jsonLength;
        if (binOffset < data.size()) {
            std::ofstream binFile("output.bin", std::ios::binary);
            binFile.write(&data[binOffset + 8], data.size() - binOffset - 8); // Skip 8-byte chunk header
        }

        // Update buffer reference in JSON
        gltfJson["buffers"][0]["uri"] = "output.bin";

        // Write JSON to .gltf file
        std::ofstream gltfFile("output.gltf");
        gltfFile << gltfJson.dump(4); // Pretty-print JSON

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
                size_t bufferIndex = bufferView["buffer"];
                size_t byteOffset = bufferView["byteOffset"];

                std::vector<char> bin = readBinaryFile(gltfJson["buffers"][bufferIndex]["uri"]);
                float* buffer = (float*)(bin.data() + byteOffset);

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