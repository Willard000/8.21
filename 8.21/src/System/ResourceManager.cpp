#include "ResourceManager.h"

#include "../src/Utility/FileReader.h"
#include "../src/Resources/Program.h"
#include "../src/Resources/Model.h"

#include "../src/System/Environment.h"
#include "../src/Resources/Window.h"
#include "../src/Resources/Camera.h"

#include "../src/Resources/Terrain.h"
#include "../src/Entities/Entity.h"

#include <iostream>

#define SHADER_FILE "Data\\Shaders\\shaders.txt"
#define MODEL_FILE "Data\\Models\\models.txt"
#define ENTITY_FILE "Data\\Entities\\entities.txt"

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
	return _programs.count(key) ? _programs[key] : std::make_shared<Program>();
}

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

ResourceManager::ResourceManager()
{}

ResourceManager::~ResourceManager() 
{}

void ResourceManager::load_resources() {
	load_programs();
	load_models();
	load_default_entities();
	load_map();
}

void ResourceManager::draw() {
	_terrain->draw(GL_TRIANGLES);
}