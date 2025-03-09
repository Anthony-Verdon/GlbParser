#include "GlbParser/GlbParser.hpp"
#include "JsonParser/JsonParser.hpp"
#include "JsonParser/JsonValue.hpp"
#include "Utils/Utils.hpp"
#include <stdexcept>
#include <iostream>
#include <fstream>

namespace Glb
{
    std::pair<JsonParser::JsonValue, std::string> LoadBinaryFile(const std::string &path, bool generateFiles)
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

    Gltf LoadGltf(const JsonParser::JsonValue &gltfJson, const std::string &binStr)
    {
        Gltf data;

        data.rootScene = (int)gltfJson["scene"];

        for (sceneJson: gltfJson["scenes"])
            data.scenes.push_back(LoadScene(sceneJson));

        for (nodeJson: gltfJson["nodes"])
            data.nodes.push_back(LoadNode(nodeJson));

        for (meshJson: gltfJson["meshes"])
            data.meshes.push_back(LoadMesh(meshJson, gltfJson, binStr));

        for (skinJson: gltfJson["skins"])
            data.skins.push_back(LoadSkin(skinJson, gltfJson, binStr));

        for (materialJson: gltfJson["materials"])
            data.materials.push_back(LoadMaterial(materialJson, gltfJson, binStr));

        for (imageJson: gltfJson["images"])
            data.images.push_back(LoadImage(imageJson, gltfJson, binStr));
    }

    Scene LoadScene(const JsonParser::JsonValue &sceneJson)
    {
        Scene scene;

        scene.name = sceneJson["name"];
        for (size_t nodeIndex: sceneJson["nodes"])
            scene.nodes.push_back(nodeIndex);

        return (scene);
    }
    
    Node LoadNode(const JsonParser::JsonValue &nodeJson)
    {
        Node node;
        
        node.name = nodeJson["name"];
        node.transform = CalculateTransform(nodeJson);

        if (nodeJson.KeyExist("children"))
        {
            for (size_t child : nodeJson["children"])
                node.children.push_back(child);
        }
    
        if (nodeJson.KeyExist("mesh"))
            node.mesh = nodeJson["mesh"];

        if (nodeJson.KeyExist("skin"))
            node.skin = nodeJson["skin"];

        return (node);
    }

    AlgOps::mat4 CalculateTransform(const JsonParser::JsonValue &nodeJson)
    {
        AlgOps::vec3 scale;
        scale.uniform(1);
        if (nodeJson.KeyExist("scale"))
            scale = {nodeJson["scale"][0], nodeJson["scale"][1], nodeJson["scale"][2]};

        AlgOps::vec3 translate;
        translate.uniform(0);
        if (nodeJson.KeyExist("translation"))
            translate = {nodeJson["translation"][0], nodeJson["translation"][1], nodeJson["translation"][2]};

        AlgOps::vec4 quat;
        quat.uniform(0);
        if (nodeJson.KeyExist("rotation"))
            quat = {nodeJson["rotation"][0], node["rotation"][1], nodeJson["rotation"][2], nodeJson["rotation"][3]};

        float roll = atan2(2 * (quat.getW() * quat.getX() + quat.getY() * quat.getZ()), 1 - 2 * (pow(quat.getX(), 2) + pow(quat.getX(), 2)));
        float pitch = asin(2 * (quat.getW() * quat.getY() - quat.getZ() * quat.getX()));
        float yaw = atan2(2 * (quat.getW() * quat.getZ() + quat.getX() * quat.getY()), 1 - 2 * (pow(quat.getY(), 2) + pow(quat.getZ(), 2)));

        AlgOps::vec3 axis[3];
        axis[0] = {1, 0, 0};
        axis[1] = {0, 1, 0};
        axis[2] = {0, 0, 1};
        
        AlgOps::mat4 rotate;
        rotate.uniform(1);
        rotate = AlgOps::rotate(rotate, roll, axis[0]) *
                AlgOps::rotate(rotate, pitch, axis[1]) *
                AlgOps::rotate(rotate, yaw, axis[2]);

        AlgOps::mat4 transform;
        transform.identity();
        transform = AlgOps::translate(model, translate)
                * rotate
                * AlgOps::scale(model, scale);  
        
        return (transform);
    }

    Mesh LoadMesh(const JsonParser::JsonValue &meshJson, const JsonParser::JsonValue &gltfJson, const std::string &binStr)
    {
        Mesh mesh;

        mesh.name = meshJson["name"];

        auto primitives = mesh["primitives"][0];

        LoadVertices(mesh, gltfJson, binStr, primitives["attributes"]);
        LoadIndices(mesh, gltfJson, binStr, primitives["indices"]);
        if (primitives.KeyExist("material"))
            mesh.material = primitives["material"];

        return (mesh);
    }

    void LoadVertices(Mesh &mesh, JsonParser::JsonValue &gltfJson, const std::string &binStr, JsonParser::JsonValue &attributes)
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
                if (componentType == GL_UNSIGNED_BYTE)
                {
                    uint8_t *data = (uint8_t*)buffer;
                    for (size_t i = 0; i < size; i++)
                        joints.push_back((uint16_t)data[i]);
                }
                else if (componentType == GL_UNSIGNED_SHORT)
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
        std::vector<VertexStruct> vertices(count);
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
        mesh.vertices = vertices;
    }

    void LoadIndices(Mesh &mesh, JsonParser::JsonValue &gltfJson, const std::string &binStr, int indiceIndex)
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
        
        mesh.indices = indices;
    }

    Skin LoadSkin(const JsonParser::JsonValue &skinJson, const JsonParser::JsonValue &gltfJson, const std::string &binStr)
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
        std::vector<AlgOps::mat4> matrices;
        matrices.reserve(count);
        for (size_t i = 0; i < count; i++)
        {
            AlgOps::mat4 matrix;
            for (size_t j = 0; j < nbFloat; j++)
                matrix.setData(j % 4, j / 4, buffer[i * nbFloat + j]);
            matrices.push_back(matrix);
        }

        size_t i = 0;
        for (auto joint: skinJson["joints"])
        {
            skin[joint] =  matrices[i];
            i++;
        }

        return (skin);
    }

    Material LoadMaterial(const JsonParser::JsonValue &materialJson, const JsonParser::JsonValue &gltfJson, const std::string &binStr)
    {
        Material material;

        material.name = materialJson["name"];
        if (materialJson["pbrMetallicRoughness"].KeyExist("baseColorTexture"))
            material.texture = materialJson["pbrMetallicRoughness"]["baseColorTexture"];
        else
            material.texture = -1;

        return (material);
    }

    Image LoadImage(const JsonParser::JsonValue &imageJson, const JsonParser::JsonValue &gltfJson, const std::string &binStr)
    {
        Image image;

        image.name = imageJson["name"];
        size_t bufferViewIndex = imageJson["bufferView"];
        auto bufferView = gltfJson["bufferViews"][bufferViewIndex];
        size_t byteOffset = bufferView["byteOffset"];
        
        image.buffer = (unsigned char*)(binStr.data() + byteOffset);
        image.bufferLength = bufferView["byteLength"];

        return (image);
    }
}