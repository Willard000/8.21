#include "Model.h"

#include "../src/Utility/FileReader.h"

#include "../src/Resources/Program.h"

#include "../src/System/Environment.h"
#include "../src/System/ResourceManager.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <SOIL/SOIL2.h>

#include <iostream>

ReadModelFile::ReadModelFile(const char* file_path) {
	FileReader file(file_path);
	if (file.set_section("Model")) {
		file.read(&_directory, "DIR");
		file.read(&_file, "file");
		file.read(&_program_key, "program");
	}
}

Model::Model() :
	_id				( 0 ),
	_program		( 0 )
{}

Model::Model(std::shared_ptr<Program> program, std::string_view directory, std::string_view model_file) :
	_id				( 0 ),
	_program		( program )
{
	load_assimp(directory, model_file);
}

Model::Model(int id, std::string_view file_path) :
	_id				( id )
{
	load_from_file(file_path.data());
}

Model::Model(const Model& rhs) :
	_id				( rhs._id ),
	_program		( rhs._program ),
	_meshes			( rhs._meshes ) 
{}

void Model::draw(Transform& transform) const {
	for(auto mesh : _meshes) {
		mesh.draw(_program->_id, transform);
	}
}

bool Model::load_from_file(const char* file_path) {
	ReadModelFile model_file(file_path);
	bool loaded = false;

	loaded = load_assimp(model_file._directory, model_file._file);

	_program = Environment::get().get_resource_manager()->get_program(model_file._program_key);
	if (_program == 0) {
		std::cout << "Warning: Shader Error -- index: " << model_file._program_key << " doesn't exist" << '\n';
	}

	return loaded;
}

std::shared_ptr<Program> Model::get_program() {
	return _program;
}

void load_material_textures(std::vector<Texture>& textures, aiMaterial* material, const aiTextureType type, const std::string_view type_name, const std::string_view directory) {
	for (unsigned int i = 0; i < material->GetTextureCount(type); ++i) {
		aiString string;
		material->GetTexture(type, i, &string);

		std::string path(string.C_Str());
		size_t end = path.find_last_of('\\') + 1;
		path.erase(0, end);
		path.insert(0, directory);
		std::cout << "Texture Path: " << path << '\n';

		Texture texture;
		texture._id = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
		if (texture._id == 0) {
			std::cout << SOIL_last_result() << '\n';
		}

		std::cout << "id: " << texture._id << '\n';
		texture._type = type_name;

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		textures.push_back(texture);
	}
}

bool Model::load_assimp(std::string_view directory, std::string_view model_file) {
	Assimp::Importer importer;
	std::string path;
	path.reserve(directory.size() + model_file.size());
	path.append(directory);
	path.append(model_file);

	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);
	if (!scene) {
		std::cout << "Assimp Loader -- Couldn't load scene at -- " << path << '\n';
		std::cout << importer.GetErrorString();
		return false;
	}

	const aiMesh* ai_mesh = scene->mMeshes[0];
	std::cout << "Meshes: " << scene->mNumMeshes << '\n';
	std::cout << "Materials: " << scene->mNumMaterials << '\n';
	std::cout << "Textures: " << scene->mNumTextures << '\n';

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
		Mesh mesh;
		ai_mesh = scene->mMeshes[i];
		mesh._vertices.reserve(ai_mesh->mNumVertices);
		mesh._uvs.reserve(ai_mesh->mNumVertices);
		mesh._normals.reserve(ai_mesh->mNumVertices);
		const aiVector3D no_texture_coord(0.0f, 0.0f, 0.0f);
		for (unsigned int i = 0; i < ai_mesh->mNumVertices; ++i) {
			aiVector3D pos = ai_mesh->mVertices[i];
			mesh._vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
			aiVector3D uv = ai_mesh->HasTextureCoords(0) ? ai_mesh->mTextureCoords[0][i] : no_texture_coord;
			mesh._uvs.push_back(glm::vec2(uv.x, uv.y));

			if (ai_mesh->HasNormals()) {
				aiVector3D normal = ai_mesh->mNormals[i];
				mesh._normals.push_back(glm::vec3(normal.x, normal.y, normal.z));
			}
		}

		mesh._indices.reserve(ai_mesh->mNumFaces);
		for (unsigned int i = 0; i < ai_mesh->mNumFaces; ++i) {
			mesh._indices.push_back(ai_mesh->mFaces[i].mIndices[0]);
			mesh._indices.push_back(ai_mesh->mFaces[i].mIndices[1]);
			mesh._indices.push_back(ai_mesh->mFaces[i].mIndices[2]);
		}

		mesh.load_buffers();

		if (ai_mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[ai_mesh->mMaterialIndex];

			std::cout << "Texture Count: " << material->GetTextureCount(aiTextureType_DIFFUSE) << '\n';
			load_material_textures(mesh._textures, material, aiTextureType_DIFFUSE, "diffuse", directory);
			load_material_textures(mesh._textures, material, aiTextureType_SPECULAR, "specular", directory);

		}

		_meshes.push_back(mesh);
	}

	return true;
}