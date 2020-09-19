#include "ResourceManager.h"

#include "../src/Utility/FileReader.h"
#include "../src/Resources/Program.h"
#include "../src/Resources/Model.h"

#include "../src/System/Environment.h"
#include "../src/Resources/Window.h"
#include "../src/Resources/Camera.h"

#include "../src/Resources/Terrain.h"
#include "../src/Entities/Entity.h"

#include "../src/Resources/Icon.h"

#include <iostream>

#define SHADER_FILE "Data\\Shaders\\shaders.txt"
#define MODEL_FILE "Data\\Models\\models.txt"
#define ENTITY_FILE "Data\\Entities\\entities.txt"
#define TEXTURE_FILE "Data\\Textures\\textures.txt"

/********************************************************************************************************************************************************/

ProgramManager::ProgramManager()
{}

ProgramManager::~ProgramManager()
{}

void ProgramManager::load_programs() {
	FileReader file(SHADER_FILE, FileReader::int_val);

	if(!file.is_read()) {
		//...
		assert(NULL);
	}

	for(auto it = file.begin(); it != file.end(); ++it) {
		for(auto itt = it->table.begin(); itt != it->table.end(); ++itt) {
			std::cout << "Load Program -- " << itt->value << '\n';
			load_program(itt->key_val, itt->value);
		}
	}
}

bool ProgramManager::load_program(int key, std::string_view file_path) {
	if(_programs.count(key)) {
		std::cout << "Duplicate Program Key -- " << key << '\n';
		return false;
	}

	const auto program = std::make_shared<Program>(key, file_path.data());
	_programs[key] = program;

	Environment::get().get_window()->get_camera()->attach_shader(program->_id);

	return true;
}

std::shared_ptr<Program> ProgramManager::get_program(int key) {
	return _programs.at(key);
}

/********************************************************************************************************************************************************/

TextureManager::TextureManager()
{}

TextureManager::~TextureManager()
{}

void TextureManager::load_textures() {
	FileReader file(TEXTURE_FILE, FileReader::int_val);

	if (!file.is_read()) {
		//...
		assert(NULL);
	}

	for(auto it = file.begin(); it != file.end(); ++it) {
		for(auto itt = it->table.begin(); itt != it->table.end(); ++itt) {

			if (it->section == "Texture") {
				std::cout << "Load Texture -- " << itt->value << '\n';
				load_texture(itt->key_val, itt->value);
			}
			
			if(it->section == "Icons") {
				std::cout << "Load Icon -- " << itt->value << '\n';
				load_icon(itt->key_val, itt->value);
			}
		}
	}
}

bool TextureManager::load_texture(int key, std::string_view file_path) {
	if (_textures.count(key)) {
		std::cout << "Duplicate Texture Key -- " << key << '\n';
		return false;
	}

	const auto texture = std::make_shared<Texture>(key, file_path.data());
	_textures[key] = texture;

	return true;
}

std::shared_ptr<Texture> TextureManager::get_texture(int key) {
	return _textures.at(key);
}

bool TextureManager::load_icon(int key, std::string_view file_path) {
	if (_icons.count(key)) {
		std::cout << "Duplicate Icon Key -- " << key << '\n';
		return false;
	}

	const auto icon = std::make_shared<GUIIcon>(key, file_path.data());
	_icons[key] = icon;

	return true;
}

std::shared_ptr<GUIIcon> TextureManager::get_icon(int key) {
	return _icons.at(key);
}

std::map<int, std::shared_ptr<GUIIcon>>* TextureManager::get_icons() {
	return &_icons;
}

/********************************************************************************************************************************************************/

MapManager::MapManager()
{}

MapManager::~MapManager()
{}

void MapManager::load_map() {
	_terrain = std::make_shared<Terrain>(100, 100, 1.0f, 1.0f);
}

std::shared_ptr<Terrain> MapManager::get_terrain() {
	return _terrain;
}

/********************************************************************************************************************************************************/

ModelManager::ModelManager()
{}

ModelManager::~ModelManager()
{}

void ModelManager::load_models() {
	FileReader file(MODEL_FILE, FileReader::int_val);

	if(!file.is_read()) {
		//...
		assert(NULL);
	}

	for(auto it = file.begin(); it != file.end(); ++it) {
		for(auto itt = it->table.begin(); itt != it->table.end(); ++itt) {
			std::cout << "Load Model " << itt->value << '\n';
			load_model(itt->key_val, itt->value);
		}
	}
}

bool ModelManager::load_model(int id, std::string_view file_path) {
	if (_models.count(id)) {
		std::cout << "Duplicate Model ID -- " << id << '\n';
		return false;
	}

	FileReader file(file_path.data());
	if (!file.is_read()) {
		return false;
	}

	const auto model = std::make_shared<Model>(id, file_path);
	_models[id] = model;

	return true;
}

std::shared_ptr<Model> ModelManager::get_model(int key) {
	return _models.at(key);
}

/********************************************************************************************************************************************************/

EntityManager::EntityManager()
{}

EntityManager::~EntityManager()
{}

void EntityManager::load_default_entities() {
	FileReader file(ENTITY_FILE, FileReader::int_val);

	if(!file.is_read()) {
		//...
		assert(NULL);
	}

	for (auto it = file.begin(); it != file.end(); ++it) {
		for(auto itt = it->table.begin(); itt != it->table.end(); ++itt) {
			std::cout << "Load Entity: " << itt->value << '\n';

			if(_default_entities[it->section].count(itt->key_val)) {
				std::cout << "Duplicate Entity ID " << '\n';
			}

			const auto entity = std::make_shared<Entity>(it->section, itt->key_val);
			entity->load();
			_default_entities[it->section][itt->key_val] = entity;
		}
	}

}

std::shared_ptr<Entity> EntityManager::new_entity(std::string_view type, int id) {
	const auto default_entity = _default_entities.at(type.data()).at(id);
	const auto new_entity = std::make_shared<Entity>(*default_entity);
	new_entity->copy(*default_entity);

	_entities.push_back(new_entity);

	return new_entity;
}

std::shared_ptr<Entity> EntityManager::get_default_entity(std::string_view type, unsigned int id) {
	return _default_entities.at(type.data()).at(id);
}

/********************************************************************************************************************************************************/

ResourceManager::ResourceManager()
{}

ResourceManager::~ResourceManager() 
{}

void ResourceManager::load_resources() {
	load_programs();
	load_textures();
	load_models();
	load_default_entities();
	load_map();
}

void ResourceManager::draw() {
	_terrain->draw(GL_TRIANGLES);

	for(const auto entity : _entities) {
		_models.at(entity->get_model_id())->draw(entity->get<TransformComponent>()->_transform);
	}
}
/********************************************************************************************************************************************************/