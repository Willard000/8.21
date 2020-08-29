#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <array>
#include <vector>
#include <map>
#include <string>

#include <memory>

struct Program;
class Model;
class Terrain;
class Entity;

class ProgramManager {
public:
public:
	ProgramManager();
	~ProgramManager();

	std::shared_ptr<Program> get_program(int key);
protected:
	void load_programs();
	bool load_program(int key, std::string_view file_path);
private:

	std::map<int, std::shared_ptr<Program>> _programs;
};

class MapManager {
public:
	MapManager();
	~MapManager();

	std::shared_ptr<Terrain> get_terrain();
protected:
	void load_map();
	std::shared_ptr<Terrain> _terrain;
private:
};

class ModelManager {
public:
	ModelManager();
	~ModelManager();

	void load_models();
	bool load_model(int id, std::string_view file_path);
protected:
	std::map<int, std::shared_ptr<Model>> _models;
private:
};

class EntityManager {
public:
	EntityManager();
	~EntityManager();

	void load_default_entities();
protected:
	std::vector<std::shared_ptr<Entity>> _entities;

	// <Type, <ID, Entity>>
	std::map<std::string, std::map<unsigned int, std::shared_ptr<Entity>>> _default_entities;
private:
};

class ResourceManager : public ProgramManager, public ModelManager, public MapManager, public EntityManager{
public:
	ResourceManager();
	~ResourceManager();


	void load_resources();
	void draw();
private:
};

#endif