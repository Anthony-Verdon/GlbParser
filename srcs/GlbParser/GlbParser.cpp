#include "GlbParser/GlbParser.hpp"
#include "JsonParser/JsonParser.hpp"
#include "JsonParser/JsonValue.hpp"
#include "Toolbox.hpp"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include "geometry/geometry.hpp"
namespace Glb
{
    std::pair<JsonParser::JsonValue, std::string> LoadBinaryFile(const std::string &path, bool generateFiles)
    {
        if (!Toolbox::checkExtension(path, ".glb"))
            throw(std::runtime_error("wrong extension, only parse .glb file"));

        std::string data = Toolbox::readFile(path, std::ios::binary);

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

    GltfData LoadGltf(JsonParser::JsonValue &gltfJson, const std::string &binStr)
    {
        GltfData data;

        data.rootScene = gltfJson["scene"];

        for (auto sceneJson: gltfJson["scenes"])
            data.scenes.push_back(LoadScene(sceneJson));

        for (auto nodeJson: gltfJson["nodes"])
            data.nodes.push_back(LoadNode(nodeJson));

        for (auto meshJson: gltfJson["meshes"])
            data.meshes.push_back(LoadMesh(meshJson, gltfJson, binStr));

        if (gltfJson.KeyExist("skins"))
        {
            for (auto skinJson: gltfJson["skins"])
                data.skins.push_back(LoadSkin(skinJson, gltfJson, binStr));
        }

        if (gltfJson.KeyExist("materials"))
        {
            for (auto materialJson: gltfJson["materials"])
                data.materials.push_back(LoadMaterial(materialJson));
        }

        if (gltfJson.KeyExist("images"))
        {
            for (auto imageJson: gltfJson["images"])
                data.images.push_back(LoadImage(imageJson, gltfJson, binStr));
        }

        if (gltfJson.KeyExist("animations"))
        {
            for (auto animationJson: gltfJson["animations"])
                data.animations.push_back(LoadAnimation(animationJson, gltfJson, binStr));
        }

        return (data);
    }

    Scene LoadScene(JsonParser::JsonValue &sceneJson)
    {
        Scene scene;

        scene.name = std::string(sceneJson["name"]);
        for (size_t nodeIndex: sceneJson["nodes"])
            scene.nodes.push_back(nodeIndex);

        return (scene);
    }
    
    Node LoadNode(JsonParser::JsonValue &nodeJson)
    {
        Node node;
        
        node.name = std::string(nodeJson["name"]);
        node.transform = CalculateTransform(nodeJson);

        if (nodeJson.KeyExist("children"))
        {
            for (size_t child : nodeJson["children"])
                node.children.push_back(child);
        }
    
        if (nodeJson.KeyExist("mesh"))
            node.mesh = nodeJson["mesh"];
        else
            node.mesh = -1;

        if (nodeJson.KeyExist("skin"))
            node.skin = nodeJson["skin"];
        else
            node.skin = -1;

        return (node);
    }

    ml::mat4 CalculateTransform(JsonParser::JsonValue &nodeJson)
    {
        ml::vec3 scale(1, 1, 1);
        if (nodeJson.KeyExist("scale"))
            scale = {nodeJson["scale"][0], nodeJson["scale"][1], nodeJson["scale"][2]};

        ml::vec3 translate(0, 0, 0);
        if (nodeJson.KeyExist("translation"))
            translate = {nodeJson["translation"][0], nodeJson["translation"][1], nodeJson["translation"][2]};

        ml::vec4 quat(0, 0, 0, 0);
        if (nodeJson.KeyExist("rotation"))
            quat = {nodeJson["rotation"][0], nodeJson["rotation"][1], nodeJson["rotation"][2], nodeJson["rotation"][3]};

        ml::mat4 transform = ml::translate(translate)
                * ml::rotate(quat)
                * ml::scale(scale);  

        return (transform);
    }

    Mesh LoadMesh(JsonParser::JsonValue &meshJson, JsonParser::JsonValue &gltfJson, const std::string &binStr)
    {
        Mesh mesh;

        mesh.name = std::string(meshJson["name"]);
        for (auto primitiveJson: meshJson["primitives"])
            mesh.primitives.push_back(LoadPrimitive(primitiveJson, gltfJson, binStr));

        return (mesh);
    }

    Primitive LoadPrimitive(JsonParser::JsonValue &primitiveJson, JsonParser::JsonValue &gltfJson, const std::string &binStr)
    {
        Primitive primitive;

        LoadVertices(primitive, gltfJson, binStr, primitiveJson["attributes"]);
        LoadIndices(primitive, gltfJson, binStr, primitiveJson["indices"]);
        if (primitiveJson.KeyExist("material"))
            primitive.material = primitiveJson["material"];
        else
            primitive.material = -1;

        return (primitive);
    }

    void LoadVertices(Primitive &primitive, JsonParser::JsonValue &gltfJson, const std::string &binStr, JsonParser::JsonValue &attributes)
    {
        std::vector<float> positions;
        std::vector<float> textureCoords;
        std::vector<float> normals;
        std::vector<uint16_t> joints;
        std::vector<float> weights;

        for (auto it = attributes.begin(); it != attributes.end(); it++)
        {
            auto accessor = gltfJson["accessors"][it.value()];
            size_t bufferViewIndex = accessor["bufferView"];
            size_t count = accessor["count"];
            size_t componentType = accessor["componentType"];
            std::string type = accessor["type"];
            
            size_t nbFloat = 0;
            if (type == "VEC2")
                nbFloat = 2;
            else if (type == "VEC3")
                nbFloat = 3;
            else if (type == "VEC4")
                nbFloat = 4;
            else
                std::cerr << "type unknown: " << type << std::endl;
            
            auto bufferView = gltfJson["bufferViews"][bufferViewIndex];
            size_t byteOffset = bufferView["byteOffset"];
            void* buffer = (void*)(binStr.data() + byteOffset);

            size_t size = count * nbFloat;
            if (it.key() == "POSITION")
            {
                float *data = (float*)buffer;
                positions.reserve(size);
                for (size_t i = 0; i < size; i++)
                    positions.push_back(data[i]);
            }
            else if (it.key() == "TEXCOORD_0")
            {
                float *data = (float*)buffer;
                textureCoords.reserve(size);
                for (size_t i = 0; i < size; i++)
                    textureCoords.push_back(data[i]);
            }
            else if (it.key() == "NORMAL")
            {
                float *data = (float*)buffer;
                normals.reserve(size);
                for (size_t i = 0; i < size; i++)
                    normals.push_back(data[i]);
            }
            else if (it.key() == "JOINTS_0")
            {
                joints.reserve(size);
                if (componentType == 5121) // GL_UNSIGNED_BYTE
                {
                    uint8_t *data = (uint8_t*)buffer;
                    for (size_t i = 0; i < size; i++)
                        joints.push_back((uint16_t)data[i]);
                }
                else if (componentType == 5123) // GL_UNSIGNED_SHORT
                {
                    uint16_t *data = (uint16_t*)buffer;
                    for (size_t i = 0; i < size; i++)
                        joints.push_back(data[i]);
                }
            }
            else if (it.key() == "WEIGHTS_0")
            {
                float *data = (float*)buffer;
                weights.reserve(size);
                for (size_t i = 0; i < size; i++)
                    weights.push_back(data[i]);
            }
        }
        
        size_t count = positions.size() / 3;
        std::vector<Vertex> vertices(count);
        for (size_t i = 0; i < count; i++)
        {
            vertices[i].x = positions[i * nbFloatPerPosition + 0];
            vertices[i].y = positions[i * nbFloatPerPosition + 1];
            vertices[i].z = positions[i * nbFloatPerPosition + 2];
            if (textureCoords.size() != 0)
            {
                vertices[i].u = textureCoords[i * nbFloatPerTexCoord + 0];
                vertices[i].v = textureCoords[i * nbFloatPerTexCoord + 1];
            }
            if (normals.size() != 0)
            {
                vertices[i].nx = normals[i * nbFloatPerNormal + 0];
                vertices[i].ny = normals[i * nbFloatPerNormal + 1];
                vertices[i].nz = normals[i * nbFloatPerNormal + 2];
            }
            if (joints.size() != 0)
            {
                vertices[i].j1 = joints[i * nbFloatPerJoint + 0];
                vertices[i].j2 = joints[i * nbFloatPerJoint + 1];
                vertices[i].j3 = joints[i * nbFloatPerJoint + 2];
                vertices[i].j4 = joints[i * nbFloatPerJoint + 3];
            }
            if (weights.size() != 0)
            {
                vertices[i].w1 = weights[i * nbFloatPerWeight + 0];
                vertices[i].w2 = weights[i * nbFloatPerWeight + 1];
                vertices[i].w3 = weights[i * nbFloatPerWeight + 2];
                vertices[i].w4 = weights[i * nbFloatPerWeight + 3];
            }

        }
        primitive.vertices = vertices;
    }

    void LoadIndices(Primitive &primitive, JsonParser::JsonValue &gltfJson, const std::string &binStr, int indiceIndex)
    {
        auto accessor = gltfJson["accessors"][indiceIndex];
        size_t bufferViewIndex = accessor["bufferView"];
        size_t count = accessor["count"];
        
        auto bufferView = gltfJson["bufferViews"][bufferViewIndex];
        size_t byteOffset = bufferView["byteOffset"];
        uint16_t* buffer = (uint16_t*)(binStr.data() + byteOffset);
        std::vector<uint16_t> indices;
        for (size_t i = 0; i < count; i++)
            indices.push_back(buffer[i]);
        
        primitive.indices = indices;
    }

    Skin LoadSkin(JsonParser::JsonValue &skinJson, JsonParser::JsonValue &gltfJson, const std::string &binStr)
    {
        Skin skin;

        size_t matrixIndex = skinJson["inverseBindMatrices"];

        auto accessor = gltfJson["accessors"][matrixIndex];
        size_t bufferViewIndex = accessor["bufferView"];
        size_t count = accessor["count"];
        
        auto bufferView = gltfJson["bufferViews"][bufferViewIndex];
        size_t byteOffset = bufferView["byteOffset"];
        float* buffer = (float*)(binStr.data() + byteOffset);
        
        size_t nbFloat = 16;
        std::vector<ml::mat4> matrices;
        matrices.reserve(count);
        for (size_t i = 0; i < count; i++)
        {
            ml::mat4 matrix;
            for (size_t j = 0; j < nbFloat; j++)
                matrix[j % 4][j / 4] = buffer[i * nbFloat + j];
            matrices.push_back(matrix);
        }

        size_t i = 0;
        for (auto joint: skinJson["joints"])
        {
            skin.joints.push_back({joint, matrices[i]});
            i++;
        }

        return (skin);
    }

    Material LoadMaterial(JsonParser::JsonValue &materialJson)
    {
        Material material;

        if (materialJson.KeyExist("name"))
            material.name = std::string(materialJson["name"]);
        if (materialJson.KeyExist("pbrMetallicRoughness"))
            material.pbr = LoadPBR(materialJson["pbrMetallicRoughness"]);
        if (materialJson.KeyExist("normalTexture"))
            material.normalTexture = materialJson["normalTexture"]["index"];
        if (materialJson.KeyExist("occlusionTexture"))
            material.occlusionTexture = materialJson["occlusionTexture"]["index"];
        if (materialJson.KeyExist("emissiveTexture"))
            material.emissiveTexture = materialJson["emissiveTexture"]["index"];
        if (materialJson.KeyExist("emissiveFactor"))
            material.emissiveFactor = ml::vec3(materialJson["emissiveFactor"][0], materialJson["emissiveFactor"][1], materialJson["emissiveFactor"][2]);
        if (materialJson.KeyExist("alphaMode"))
            material.alphaMode = std::string(materialJson["alphaMode"]);
        if (materialJson.KeyExist("alphaCutoff"))
            material.alphaCutoff = materialJson["alphaCutoff"];
        if (materialJson.KeyExist("doubleSided"))
            material.doubleSided = materialJson["doubleSided"];

        return (material);
    }

    PbrMetallicRoughness LoadPBR(JsonParser::JsonValue &pbrJson)
    {
        PbrMetallicRoughness pbr;

        if (pbrJson.KeyExist("baseColorFactor"))
            pbr.baseColorFactor = ml::vec4(pbrJson["baseColorFactor"][0], pbrJson["baseColorFactor"][1], pbrJson["baseColorFactor"][2], pbrJson["baseColorFactor"][3]);
        if (pbrJson.KeyExist("baseColorTexture"))
            pbr.baseColorTexture = pbrJson["baseColorTexture"]["index"];
        if (pbrJson.KeyExist("metallicFactor"))
            pbr.metallicFactor = pbrJson["metallicFactor"];
        if (pbrJson.KeyExist("roughnessFactor"))
            pbr.roughnessFactor = pbrJson["roughnessFactor"];
        if (pbrJson.KeyExist("metallicRoughnessTexture"))
            pbr.metallicRoughnessTexture = pbrJson["metallicRoughnessTexture"]["index"];

        return (pbr);
    }

    Image LoadImage(JsonParser::JsonValue &imageJson, JsonParser::JsonValue &gltfJson, const std::string &binStr)
    {
        Image image;

        image.name = std::string(imageJson["name"]);
        size_t bufferViewIndex = imageJson["bufferView"];
        auto bufferView = gltfJson["bufferViews"][bufferViewIndex];
        size_t byteOffset = bufferView["byteOffset"];
        
        image.buffer = (unsigned char*)(binStr.data() + byteOffset);
        image.bufferLength = bufferView["byteLength"];

        return (image);
    }

    Animation LoadAnimation(JsonParser::JsonValue &animationJson, JsonParser::JsonValue &gltfJson, const std::string &binStr)
    {
        Animation animation;
        animation.name = std::string(animationJson["name"]);
        for (auto channelJson: animationJson["channels"])
        {
            Channel channel;
            channel.sampler = channelJson["sampler"];
            channel.node = channelJson["target"]["node"];
            channel.type = std::string(channelJson["target"]["path"]);
            
            auto samplerJson = animationJson["samplers"][channel.sampler];
            Sampler sampler;
            sampler.interpolation = std::string(samplerJson["interpolation"]);

            // input = timecodes
            {
                size_t input = samplerJson["input"];
                auto accessor = gltfJson["accessors"][input];
                size_t bufferViewIndex = accessor["bufferView"];
                size_t count = accessor["count"];

                auto bufferView = gltfJson["bufferViews"][bufferViewIndex];
                size_t byteOffset = bufferView["byteOffset"];
                float* buffer = (float*)(binStr.data() + byteOffset);

                for (size_t i = 0; i < count; i++)
                    sampler.timecodes.push_back(buffer[i]);
            }

            // output = data
            {
                size_t output = samplerJson["output"];
                auto accessor = gltfJson["accessors"][output];
                size_t bufferViewIndex = accessor["bufferView"];
                size_t count = accessor["count"];
                std::string type = accessor["type"];

                sampler.nbElement = 0;
                if (type == "SCALAR")
                    sampler.nbElement = 1;
                else if (type == "VEC2")
                    sampler.nbElement = 2;
                else if (type == "VEC3")
                    sampler.nbElement = 3;
                else if (type == "VEC4")
                    sampler.nbElement = 4;

                auto bufferView = gltfJson["bufferViews"][bufferViewIndex];
                size_t byteOffset = bufferView["byteOffset"];
                float* buffer = (float*)(binStr.data() + byteOffset);

                for (size_t i = 0; i < count * sampler.nbElement; i++)
                    sampler.data.push_back(buffer[i]);
            }
            animation.channels.push_back(channel);
            animation.samplers.push_back(sampler);
        }

        return (animation);
    }
}