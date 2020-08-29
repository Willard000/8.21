#include "Entity.h"

#include "../src/Utility/FileReader.h"

#define ENTITY_FILE "Data\\Entities\\entities.txt"

unsigned int make_unique_id() {
	static unsigned int id = 0;
	return id++;
}

ReadEntityFile::ReadEntityFile(const char* file_path) {
	FileReader file(file_path);

	if (file.set_section("Entity")) {
		file.read(&_model_id, "model_id");
		file.read(&_draw, "draw");
		file.read(&_name, "str_name");
	}

	if(file.set_section("Transform")) {
		_transform_file = std::make_shared<ReadTransformFile>(file);
	}
}

EntityLoader::EntityLoader(std::shared_ptr<Entity> entity) :
	_entity				( entity ),
	_entity_file		( find_file_path().c_str() )
{
	_entity->_name = _entity_file._name;
	_entity->_model_id = _entity_file._model_id;
	_entity->_draw = _entity_file._draw;

	if(_entity_file._transform_file) {
		load_transform();
	}
}

std::string EntityLoader::find_file_path() {
	FileReader file(ENTITY_FILE, FileReader::int_val);

	if(!file.is_read()) {
		//...
		assert(NULL);
	}

	std::string path;
	file.set_section(_entity->get_type());
	file.read(&path, _entity->get_id());

	return path;
}

// TransformComponent(std::shared_ptr<Entity> entity, glm::vec3 scale, glm::vec3 rotation, float speed, bool collidable);
void EntityLoader::load_transform() {
	const auto transform_file = _entity_file._transform_file;
	_entity->_components[TRANSFORM_COMPONENT] = std::make_shared<TransformComponent>(
		_entity,
		transform_file->_scale,
		transform_file->_rotation,
		transform_file->_speed,
		transform_file->_collidable
	);
}

Entity::Entity() :
	_unique_id		    ( make_unique_id() ),
	_id			   	    ( -1 ),
	_model_id			( 0 ),
	_destroy			( false ),
	_draw				( false )
{}

Entity::Entity(const std::string_view type, const int id) :
	_unique_id			( make_unique_id() ),
	_type				( type ),
	_id					( id ),
	_model_id			( 0 ),
	_destroy			( false ),
	_draw				( true )
{
	// ready to load
}

Entity::Entity(const Entity& rhs) :
	_unique_id			( make_unique_id() ),
	_type				( rhs._type ),
	_id					( rhs._id ),
	_model_id			( rhs._model_id ),
	_name				( rhs._name ),
	_destroy			( rhs._destroy ),
	_draw				( rhs._draw )
{
	// ready to copy
}

Entity::~Entity() 
{}

void Entity::add(std::shared_ptr<Component> component) {
	_components[component->get_type()] = component;
}

void Entity::update() {
	for(auto c : _components) {
		if (c) {
			c->update();
		}
	}
}

void Entity::clear() {
	for(auto c : _components) {
		c = nullptr;
	}
}

void Entity::copy(const Entity& rhs) {
	for (size_t i = 0; i < TOTAL_COMPONENTS; ++i) {
		if (rhs._components[i]) {
			_components[i] = rhs._components[i]->copy(shared_from_this());
		}
	}
}

void Entity::destroy() {
	_destroy = true;
}

void Entity::load() {
	EntityLoader loader(shared_from_this());
}

unsigned int Entity::get_unique_id() {
	return _unique_id;
}

int Entity::get_id() {
	return _id;
}

std::string Entity::get_type() {
	return _type;
}

int Entity::get_model_id() {
	return _model_id;
}

std::string_view Entity::get_name() {
	return _name;
}

void Entity::set_model_id(const int model_id) {
	_model_id = model_id;
}

void Entity::set_name(const std::string_view name) {
	_name = name;
}

void Entity::set_draw(bool draw) {
	_draw = draw;
}

bool Entity::get_destroy() {
	return _destroy;
}

bool Entity::get_draw() {
	return _draw;
}