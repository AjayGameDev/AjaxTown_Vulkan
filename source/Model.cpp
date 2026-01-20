#include "Model.h"

#include "FileManager.h"




Model::Model(const std::string& modelName)
{
    LoadModel(modelName);
}

Model::~Model()
{

}



void Model::LoadModel(const std::string& modelName)
{
    Assimp::Importer importer;
    std::string filePath = FileManager::GetLocation(FileManager::Model) + modelName;

    const aiScene* scene = importer.ReadFile(filePath.c_str(),aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);

    if (scene == nullptr || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
    {
        std::string error = "\nCan't load model : " + modelName + " " + importer.GetErrorString();
        spdlog::critical(error);
        throw std::runtime_error(error);
    }

    Process(scene);
    std::cout << "\nVertices: " + std::to_string(vertices.size()) + " \nIndices: " + std::to_string(indices.size());
}

void Model::Process(const aiScene* scene)
{
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];

        for (unsigned int j = 0; j < scene->mMeshes[i]->mNumVertices; j++)
        {
            aiVector3D _vertex    = mesh->mVertices[j];
            aiVector3D _normal    = mesh->mNormals[j];
            aiVector3D _tangent   = mesh->mTangents[j];
            aiVector3D _bitangent = mesh->mBitangents[j];

            Vector3 position(_vertex.x,_vertex.y,_vertex.z);
            Vector3 normal(_normal.x,_normal.y,_normal.z);
            Vector3 tangent(_tangent.x,_tangent.y,_tangent.z);
            Vector3 bitangent(_bitangent.x,_bitangent.y,_bitangent.z);

            aiVector3D textureCoordinates = mesh->mTextureCoords[0] ? mesh->mTextureCoords[0][j] : aiVector3D(0,0,0);

            float handedness = Vector3::Dot(Vector3::Cross(normal,tangent),bitangent) > .0f ? 1.0f : -1.0f;  // same order should be in shader

            vertices.push_back
            ({
                position,
                normal,
                Vector2(textureCoordinates.x,textureCoordinates.y),
                Vector4(tangent.x,tangent.y,tangent.z,handedness)
            });
        }


        for (unsigned int k = 0; k < mesh->mNumFaces; ++k)
        {
            aiFace& face = mesh->mFaces[k];

            for (int l = 0; l < face.mNumIndices; ++l)
            {
                indices.push_back(face.mIndices[l]);
            }
        }

    }
}





