#include "Entity.h"

#include "../src/Utility/FileReader.h"

#define ENTITY_FILE "Data\\Entities\\entities.txt"

unsigned int make_unique_id() {
	static unsigned int id = 0;
	return id++;
}

ReadEntityFile::ReadEntityFile(const char* file_path, std::string_view section) {
	FileReader file(file_path);

	if (file.set_section(section)) {
		file.read(&_type, "type");
		file.read(&_id, "id");
		file.read(&_model_id, "model_id");
		file.read(&_draw, "draw");
		file.read(&_name, "name");
	}

	if(file.set_section("Transform")) {
		_transform_file = std::make_shared<ReadTransformFile>(file);
	}
}

EntityLoader::EntityLoader(std::shared_ptr<Entity> entity, const char* file) :
	_entity				( entity ),
	_entity_file		( file )
{
	_entity->_type = _entity_file._type;
	_entity->_id = _entity_file._id;
	_entity->_name = _entity_file._name;
	_entity->_model_id = _entity_file._model_id;
	_entity->_draw = _entity_file._draw;

	if(_entity_file._transform_file) {
		load_transform();
	}
}

std::string EntityLoader::default_file(std::string_view type, int id) {
	FileReader file(ENTITY_FILE, FileReader::int_val);

	if(!file.is_read()) {
		//...
		assert(NULL);
	}

	std::string path;
	file.set_section(type);
	file.read(&path, id);

	return path;
}

// TransformComponent(std::shared_ptr<Entity> entity, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, float speed, bool collidable);
void EntityLoader::load_transform() {
	const auto transform_file = _entity_file._transform_file;
	_entity->_components[TRANSFORM_COMPONENT] = std::make_shared<TransformComponent>(
		_entity,
		transform_file->_position,
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
	_draw				( false ),
	_type				( "" ),
	_name				( "" )
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

Entity::~Entity() {
}

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

// load file = Default if type and id are set
// otherwise load from seperate file
void Entity::load(std::string_view file) {
	if (file == "Default") {
		const auto type = _type;
		const auto id = _id;
		EntityLoader loader(shared_from_this(), EntityLoader::default_file(_type, _id).c_str());
		// make sure type and id are correct -- ignore file
		_type = type;
		_id = id;
	}
	else {
		EntityLoader loader(shared_from_this(), file.data());
	}
}

void Entity::save(std::ofstream& file) {
	file << "# Entity" << '\n';
	file << "type " << _type << '\n';
	file << "id " << _id << '\n';
	file << "model_id " << _model_id << '\n';
	file << "name " << _name << '\n';
	file << "draw " << _draw << '\n';
	file << '\n';

	for(const auto& c : _components) {
		if(c) {
			c->save(file);
		}
	}
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

void Entity::set_unique_id(int id) {
	_unique_id = id;
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

#define ENTITY_BYTE_SIZE sizeof(int) * 3 + sizeof(bool) * 2 + STR_PADDING * 2

std::vector<PacketData> Entity::packet_data() {
	std::vector<PacketData> packet;

	packet.push_back(std::move(PacketData(_unique_id, _id, _model_id, _draw, _destroy, _type.c_str(), _name.c_str())));

	for(const auto c : _components) {
		packet.push_back(std::move(c->packet_data()));
	}

	return packet;
}

#include <iostream>
void Entity::load_buffer(void* buf, int size) {
	assert(size >= ENTITY_BYTE_SIZE);

	uint8_t* ptr = static_cast<uint8_t*>(buf);
	int byte = 0;

	std::copy(ptr, ptr + sizeof(unsigned int), &_unique_id);
	ptr += sizeof(unsigned int);
	byte += sizeof(unsigned int);

	std::copy(ptr, ptr + sizeof(int), &_id);
	ptr += sizeof(int);
	byte += sizeof(int);

	std::copy(ptr, ptr + sizeof(int), &_model_id);
	ptr += sizeof(int);
	byte += sizeof(int);

	std::copy(ptr, ptr + sizeof(bool), &_draw);
	ptr += sizeof(bool);
	byte += sizeof(bool);

	std::copy(ptr, ptr + sizeof(bool), &_destroy);
	ptr += sizeof(bool);
	byte += sizeof(bool);

	_type.assign(reinterpret_cast<const char*>(ptr));
	byte += strlen(reinterpret_cast<const char*>(ptr)) + 1;
	ptr += strlen(reinterpret_cast<const char*>(ptr)) + 1;

	_name.assign(reinterpret_cast<const char*>(ptr));
	byte += strlen(reinterpret_cast<const char*>(ptr)) + 1;
	ptr += strlen(reinterpret_cast<const char*>(ptr)) + 1;

	while (byte < size) {
		const char* component = reinterpret_cast<const char*>(ptr);
		byte += strlen(reinterpret_cast<const char*>(ptr)) + 1;
		ptr += strlen(reinterpret_cast<const char*>(ptr)) + 1;

		if(!strcmp(component, "Transform")) {
			if(!_components.at(TRANSFORM_COMPONENT)) {
				_components.at(TRANSFORM_COMPONENT) = std::make_shared<TransformComponent>(shared_from_this());
			}
			byte += _components.at(TRANSFORM_COMPONENT)->load_buffer(ptr);
		}
		else {
			assert(false); // invalid buffer component data
		}

	}
}