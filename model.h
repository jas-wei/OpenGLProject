#pragma once

#include <glad/glad.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "mesh.h"
#include "stb_image.h"


class Model
{
public:
    Model(const std::string& path)
    {
        loadModel(path);
    }

    void Draw(Shader& shader)
    {
        // A model can contain multiple meshes, so draw each one individually.
        for (Mesh& mesh : meshes)
        {
            mesh.Draw(shader);
        }
    }

private:
    // All meshes that make up the imported model.
    std::vector<Mesh> meshes;

    // Directory containing the model file. Used to find its texture files.
    std::string directory;

    // Keep track of textures that have already been loaded.
    // Some meshes can reference the same texture, so this avoids loading it twice.
    std::unordered_map<std::string, unsigned int> loadedTextures;

    vector<Texture> textures_loaded;


    void loadModel(const std::string& path)
    {
        Assimp::Importer importer;

        // Convert faces to triangles and flip UVs to match OpenGL's texture orientation.
        // Smooth normals are generated in case the model does not already contain them.
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);

        // Check that Assimp successfully loaded a usable scene.
        if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }

        // Texture paths stored in the model are usually relative to the model's folder.
        directory = std::filesystem::path(path).parent_path().string();

        // Start at the root node and work through the scene hierarchy.
        processNode(scene->mRootNode, scene);
    }


    void processNode(aiNode* node, const aiScene* scene)
    {
        // Nodes do not directly contain mesh data.
        // Instead, they store indices that point into scene->mMeshes.
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            unsigned int meshIndex = node->mMeshes[i];
            aiMesh* mesh = scene->mMeshes[meshIndex];

            meshes.push_back(processMesh(mesh, scene));
        }

        // Recursively process the node's children.
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }


    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        vertices.reserve(mesh->mNumVertices);

        // ---------------- VERTICES ----------------

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex{};

            // Position
            vertex.Position = glm::vec3(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z
            );

            // Normal
            if (mesh->HasNormals())
            {
                vertex.Normal = glm::vec3(
                    mesh->mNormals[i].x,
                    mesh->mNormals[i].y,
                    mesh->mNormals[i].z
                );
            }

            // Assimp can support multiple UV channels.
            // For now, only use the first set.
            if (mesh->mTextureCoords[0])
            {
                vertex.TexCoords = glm::vec2(
                    mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y
                );
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.0f);
            }

            vertices.push_back(vertex);
        }


        // ---------------- INDICES ----------------

        // A face contains the indices of the vertices that form it.
        // Since aiProcess_Triangulate is enabled, these should be triangles.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            const aiFace& face = mesh->mFaces[i];

            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }


        // ---------------- MATERIAL ----------------

        if (mesh->mMaterialIndex < scene->mNumMaterials)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            //Find Diffuse Map
            std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");

            if (diffuseMaps.empty())
            {
                aiColor3D diffuseColor(1.0f, 1.0f, 1.0f);

                if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS)
                {
                    Texture texture;
                    texture.id = TextureFromColor(diffuseColor);
                    texture.type = "texture_diffuse";
                    texture.path = "";

                    diffuseMaps.push_back(texture);
                }
            }

            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            //Find Specular Map
            std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");

            if (specularMaps.empty())
            {
                aiColor3D specularColor(0.0f, 0.0f, 0.0f);

                if (material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor) == AI_SUCCESS)
                {
                    Texture texture;
                    texture.id = TextureFromColor(specularColor);
                    texture.type = "texture_specular";
                    texture.path = "";

                    specularMaps.push_back(texture);
                }
            }

            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }


        return Mesh(vertices, indices, textures);
    }


    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }
            if (!skip)
            {   // if texture hasn't been loaded already, load it
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture); // add to loaded textures
            }
        }
        return textures;
    }

    unsigned int TextureFromFile(const char* path, const std::string& directory)
    {
        std::filesystem::path filename = std::filesystem::path(directory) / path;

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width;
        int height;
        int nrComponents;

        unsigned char* data = stbi_load(filename.string().c_str(), &width, &height, &nrComponents, 0);

        if (data)
        {
            GLenum format = GL_RGB;

            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 2)
                format = GL_RG;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << filename.string() << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }

    unsigned int TextureFromColor(aiColor3D color)
    {
        unsigned char pixel[] = {
            static_cast<unsigned char>(color.r * 255.0f),
            static_cast<unsigned char>(color.g * 255.0f),
            static_cast<unsigned char>(color.b * 255.0f)
        };

        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, pixel);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        return textureID;
    }
};