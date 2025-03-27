#pragma once

#include <string>
#include <map>
#include "Json/Json.hpp"
#include "Matrix/Matrix.hpp"

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

    // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-mesh-primitive
    struct Primitive
    {
        std::vector<Vertex> vertices; // attributes
        std::vector<uint16_t> indices;
        int material;
        // int mode;
        // object targets
    };

    // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-mesh
    struct Mesh
    {
        std::string name;
        std::vector<Primitive> primitives;
        // std::vector<float> weights;
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

    std::pair<Json::Node, std::string> LoadBinaryFile(const std::string &path, bool generateFiles = false);
    GltfData LoadGltf(Json::Node &gltfJson, const std::string &binStr);
    Scene LoadScene(Json::Node &sceneJson);
    Node LoadNode(Json::Node &nodeJson);
    ml::mat4 CalculateTransform(Json::Node &nodeJson);
    Mesh LoadMesh(Json::Node &meshJson, Json::Node &gltfJson, const std::string &binStr);
    Primitive LoadPrimitive(Json::Node &primitiveJson, Json::Node &gltfJson, const std::string &binStr);
    void LoadVertices(Primitive &primitive, Json::Node &gltfJson, const std::string &binStr, Json::Node &attributes);
    void LoadIndices(Primitive &primitive, Json::Node &gltfJson, const std::string &binStr, int indiceIndex);
    Skin LoadSkin(Json::Node &skinJson, Json::Node &gltfJson, const std::string &binStr);
    Material LoadMaterial(Json::Node &materialJson);
    PbrMetallicRoughness LoadPBR(Json::Node &pbrJson);
    Image LoadImage(Json::Node &imageJson, Json::Node &gltfJson, const std::string &binStr);
    Animation LoadAnimation(Json::Node &animationJson, Json::Node &gltfJson, const std::string &binStr);
}