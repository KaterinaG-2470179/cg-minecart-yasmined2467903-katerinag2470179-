#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "ModelMesh.h"

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

class Model
{
	public:
		Model(const char* path)
		{
			loadModel(path);
		}

		void Draw(ShaderProgram& shader);

	private:
		// model data
		std::vector<ModelMesh> meshes;
		std::vector<Texture> textures_loaded;
		std::string directory;

		void loadModel(std::string path);
		void processNode(aiNode* node, const aiScene* scene);

		ModelMesh processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};