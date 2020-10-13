#include "TransformComponent.h"

#include "../src/Entities/Entity.h"

#include "../src/System/Environment.h"
#include "../src/Utility/Clock.h"
#include "../src/System/ResourceManager.h"
#include "../src/Resources/Terrain.h"

#include "../src/Utility/FileReader.h"

#include <sstream>

constexpr float ROTATION_SPEED = 100000.0f;

ReadTransformFile::ReadTransformFile(FileReader& file, std::string_view section) {
	if (file.set_section(section)) {
		std::string data;

		if(file.read(&data, "position")) {
			std::stringstream ss_position(data);
			ss_position >> _position.x >> _position.y >> _position.z;
		}

		if(file.read(&data, "scale")) {
			std::stringstream ss_scale(data);
			ss_scale >> _scale.x >> _scale.y >> _scale.z;
		}

		if(file.read(&data, "rotation")) {
			std::stringstream ss_rotation(data);
			ss_rotation >> _rotation.x >> _rotation.y >> _rotation.z;
		}

		file.read(&_speed, "speed");
		file.read(&_collidable, "collidable");
	}
}

TransformComponent::TransformComponent(std::shared_ptr<Entity> entity, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, float speed, bool collidable) :
	Component				( entity ),
	_transform				( position, scale, rotation ),
	_speed					( speed ),
	_collidable				( collidable ),
	_direction				( glm::vec3(0, 0, 0) ),
	_destination			( glm::vec3(0, 0, 0) ),
	_dest_reached			( true ),
	_y_rot					( 0 ),
	_turn					( 0 )
{
	const auto c_box = Environment::get().get_resource_manager()->get_model(_entity->get_model_id())->get_collision_box();
	_collision_box.min = _transform.get_scale() * c_box.min;
	_collision_box.max = _transform.get_scale() * c_box.max;
}

TransformComponent::TransformComponent(std::shared_ptr<Entity> new_entity, const TransformComponent& rhs) :
	Component				( new_entity ),
	_transform				( rhs._transform ),
	_speed					( rhs._speed ),
	_collidable				( rhs._collidable),
	_direction				( rhs._direction ),
	_destination			( rhs._destination ),
	_dest_reached			( rhs._dest_reached ),
	_y_rot					( rhs._y_rot ),
	_turn					( rhs._turn ),
	_collision_box			( rhs._collision_box )
{}

std::shared_ptr<Component> TransformComponent::copy(std::shared_ptr<Entity> new_entity) const {
	return std::static_pointer_cast<Component>(std::make_shared<TransformComponent>(new_entity, *this));
}

void TransformComponent::update() {
	if(!_dest_reached) {
		move(_destination - _transform.get_position());
	}
	/*
	auto rotation = _transform.get_rotation();
	if (_y_rot != rotation.y) {
		if (_turn == TURN_CLOCKWISE) {
			rotation.y -= ROTATION_SPEED * (float)Environment::get().get_clock()->get_time();
			if (rotation.y < 0.0f) {
				rotation.y += 360.0f;
			}
			if (rotation.y < _y_rot) {
				rotation.y = _y_rot;
			}
		}
		else if (_turn == TURN_CCLOCKWISE) {
			rotation.y += ROTATION_SPEED * (float)Environment::get().get_clock()->get_time();
			if (rotation.y > 360.0f) {
				rotation.y -= 360.0f;
			}
			if (rotation.y > _y_rot) {
				rotation.y = _y_rot;
			}
		}
		_transform.set_rotation(rotation);
	}
	*/
}

const int TransformComponent::get_type() const {
	return _type;
}

void TransformComponent::save(std::ofstream& file) {
	const auto position = _transform.get_position();
	const auto scale = _transform.get_scale();
	const auto rotation = _transform.get_rotation();

	file << "# Transform" << '\n';

	file << "position " << position.x << " " << position.y << " " << position.z << '\n';
	file << "scale " << scale.x << " " << scale.y << " " << scale.z << '\n';
	file << "rotation " << rotation.x << " " << rotation.y << " " << rotation.z << '\n';
	file << "speed " << _speed << '\n';
	file << "collidable " << _collidable << '\n';
	file << '\n';
}

void TransformComponent::move(glm::vec3 dir) {
	dir = glm::normalize(dir);
	glm::vec3 old_position = _transform.get_position();

	const auto terrain = Environment::get().get_resource_manager()->get_terrain();
	const float x = _transform.get_position().x + dir.x * _speed;
	const float z = _transform.get_position().z + dir.z * _speed;
	const float y = terrain->exact_height(x, z);

	_transform.set_position(glm::vec3(x, y, z));

	/*
	if(_entity->is_collision()) {
		_transform.set_position(old_position);
	}
	else {
		_direction = dir;
		update_grid(old_position);
	}
	*/
}

void TransformComponent::set(glm::vec3 pos) {
	_transform.set_position(pos);
}

void TransformComponent::set_direction(glm::vec3 dir) {
	float old_y_rot = _y_rot;
	_direction = dir;

	if (dir.z < 0 && dir.x < 0) {
		_y_rot = 180.0f - dir.x * 90.0f;
	}
	else if (dir.z < 0 && dir.x > 0) {
		_y_rot = 180.0f - dir.x * 90.0f;
	}
	else if (dir.z > 0 && dir.x < 0) {
		_y_rot = 360.0f + dir.x * 90.0f;
	}
	else {
		_y_rot = dir.x * 90.0f;
	}

	float dif = old_y_rot - _y_rot;
	if(dif < 0.0f) {
		_turn = TURN_CCLOCKWISE;
	}
	else {
		_turn = TURN_CLOCKWISE;
	}
}

void TransformComponent::set_destination(glm::vec3 dest) {
	_destination = dest;
	_dest_reached = false;
}

CollisionBox TransformComponent::get_collision_box() {
	return { _collision_box.max * _transform.get_scale() + _transform.get_position(),
		     _collision_box.min * _transform.get_scale() + _transform.get_position()
	};
}