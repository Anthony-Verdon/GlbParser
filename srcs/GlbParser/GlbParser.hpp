#pragma once

#include <string>
#include <map>
#include "JsonParser/JsonValue.hpp"
#include "Matrix/Matrix.hpp"

namespace Glb
{
    struct Node
    {
        std::string name;
        AlgOps::mat4 transform;
        std::vector<int> children;
        int mesh;
        int skin;
    };

    struct Vertex
    {
        float x,y,z;
        float u,v;
        float nx, ny, nz;
        uint16_t j1, j2, j3, j4;
        float w1, w2, w3, w4;
    };

    struct Mesh
    {
        std::string name;
        std::vector<Vertex> vertices;
        std::vector<uint16_t> indices;
        int material;
    };

    struct Material
    {
        std::string name;
        int texture;
    };

    struct Image
    {
        std::string name;
        unsigned char *buffer;
        size_t bufferLength;
    };

    struct Skin
    {
        std::string name;
        std::map<int, AlgOps::mat4> joints;
    };

    struct Scene
    {
        std::string name;
        std::vector<int> nodes;
    };

    struct Gltf
    {
        int rootScene;
        std::vector<Scene> scenes;
        std::vector<Node> nodes;
        std::vector<Mesh> meshes;
        std::vector<Skin> skins;
        std::vector<Material> materials;
        std::vector<Image> images;
    };

    std::pair<JsonParser::JsonValue, std::string> LoadBinaryFile(const std::string &path, bool generateFiles = false);
    Gltf LoadGltf(const JsonParser::JsonValue &gltfJson, const std::string &binStr);
    Scene LoadScene(const JsonParser::JsonValue &sceneJson);
    Node LoadNode(const JsonParser::JsonValue &nodeJson);
    AlgOps::mat4 CalculateTransform(const JsonParser::JsonValue &nodeJson);
    Mesh LoadMesh(const JsonParser::JsonValue &meshJson, const JsonParser::JsonValue &gltfJson, const std::string &binStr);
    void LoadVertices(Mesh &mesh, JsonParser::JsonValue &gltfJson, const std::string &binStr, JsonParser::JsonValue &attributes);
    void LoadIndices(Mesh &mesh, JsonParser::JsonValue &gltfJson, const std::string &binStr, int indiceIndex);
    Skin LoadSkin(const JsonParser::JsonValue &skinJson, const JsonParser::JsonValue &gltfJson, const std::string &binStr);
    Material LoadMaterial(const JsonParser::JsonValue &materialJson, const JsonParser::JsonValue &gltfJson, const std::string &binStr);
    Image LoadImage(const JsonParser::JsonValue &imageJson, const JsonParser::JsonValue &gltfJson, const std::string &binStr);
}