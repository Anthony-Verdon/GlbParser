#pragma once

#include <string>
#include <map>
#include "JsonParser/JsonValue.hpp"
#include "vec/vec.hpp"
#include "mat/mat.hpp"

namespace Glb
{
    struct Channel
    {
        int sampler;
        int node;
        std::string type;
    };

    struct Sampler
    {
        std::vector<float> timecodes;
        std::vector<float> data;
        size_t nbElement;
        std::string interpolation;
    };

    struct Animation
    {
        std::string name;
        std::vector<Channel> channels;
        std::vector<Sampler> samplers;
    };

    struct Node
    {
        std::string name;
        ml::mat4 transform;
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

    // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-material-pbrmetallicroughness
    struct PbrMetallicRoughness
    {
        ml::vec4 baseColorFactor;
        int baseColorTexture;
        float metallicFactor;
        float roughnessFactor;
        int metallicRoughnessTexture;

        PbrMetallicRoughness()
        {
            baseColorFactor = ml::vec4(1, 1, 1, 1);
            baseColorTexture = -1;
            metallicFactor = 1;
            roughnessFactor = 1;
            metallicRoughnessTexture = -1;
        }
    };

    // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-material
    struct Material
    {
        std::string name;
        PbrMetallicRoughness pbr;
        int normalTexture;
        int occlusionTexture;
        int emissiveTexture;
        ml::vec3 emissiveFactor;
        std::string alphaMode;
        float alphaCutoff;
        bool doubleSided;

        Material()
        {
            name = "";
            normalTexture = -1;
            occlusionTexture = -1;
            emissiveTexture = -1;
            emissiveFactor = ml::vec3(0, 0, 0);
            alphaMode = "OPAQUE";
            alphaCutoff = 0.5f;
            doubleSided = false;
        }
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
        ml::mat4 inverseBindMatrix;
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
        std::vector<Animation> animations;
    };

    std::pair<JsonParser::JsonValue, std::string> LoadBinaryFile(const std::string &path, bool generateFiles = false);
    GltfData LoadGltf(JsonParser::JsonValue &gltfJson, const std::string &binStr);
    Scene LoadScene(JsonParser::JsonValue &sceneJson);
    Node LoadNode(JsonParser::JsonValue &nodeJson);
    ml::mat4 CalculateTransform(JsonParser::JsonValue &nodeJson);
    Mesh LoadMesh(JsonParser::JsonValue &meshJson, JsonParser::JsonValue &gltfJson, const std::string &binStr);
    void LoadVertices(Mesh &mesh, JsonParser::JsonValue &gltfJson, const std::string &binStr, JsonParser::JsonValue &attributes);
    void LoadIndices(Mesh &mesh, JsonParser::JsonValue &gltfJson, const std::string &binStr, int indiceIndex);
    Skin LoadSkin(JsonParser::JsonValue &skinJson, JsonParser::JsonValue &gltfJson, const std::string &binStr);
    Material LoadMaterial(JsonParser::JsonValue &materialJson);
    PbrMetallicRoughness LoadPBR(JsonParser::JsonValue &pbrJson);
    Image LoadImage(JsonParser::JsonValue &imageJson, JsonParser::JsonValue &gltfJson, const std::string &binStr);
    Animation LoadAnimation(JsonParser::JsonValue &animationJson, JsonParser::JsonValue &gltfJson, const std::string &binStr);
}