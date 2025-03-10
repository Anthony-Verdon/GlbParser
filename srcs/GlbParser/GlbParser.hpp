#pragma once

#include <string>
#include <map>
#include "JsonParser/JsonValue.hpp"
#include "Matrix.hpp"

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

    constexpr int nbFloatPerPosition = 3;
    constexpr int nbFloatPerTexCoord = 2;
    constexpr int nbFloatPerNormal = 3;
    constexpr int nbFloatPerJoint = 4;
    constexpr int nbFloatPerWeight = 4;

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
        int image;
    };

    struct Image
    {
        std::string name;
        unsigned char *buffer;
        size_t bufferLength;
    };

    struct Joint
    {
        int nodeIndex;
        AlgOps::mat4 inverseBindMatrix;
    };

    struct Skin
    {
        std::string name;
        std::vector<Joint> joints;
    };

    struct Scene
    {
        std::string name;
        std::vector<int> nodes;
    };

    struct GltfData
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
    GltfData LoadGltf(JsonParser::JsonValue &gltfJson, const std::string &binStr);
    Scene LoadScene(JsonParser::JsonValue &sceneJson);
    Node LoadNode(JsonParser::JsonValue &nodeJson);
    AlgOps::mat4 CalculateTransform(JsonParser::JsonValue &nodeJson);
    Mesh LoadMesh(JsonParser::JsonValue &meshJson, JsonParser::JsonValue &gltfJson, const std::string &binStr);
    void LoadVertices(Mesh &mesh, JsonParser::JsonValue &gltfJson, const std::string &binStr, JsonParser::JsonValue &attributes);
    void LoadIndices(Mesh &mesh, JsonParser::JsonValue &gltfJson, const std::string &binStr, int indiceIndex);
    Skin LoadSkin(JsonParser::JsonValue &skinJson, JsonParser::JsonValue &gltfJson, const std::string &binStr);
    Material LoadMaterial(JsonParser::JsonValue &materialJson);
    Image LoadImage(JsonParser::JsonValue &imageJson, JsonParser::JsonValue &gltfJson, const std::string &binStr);
}