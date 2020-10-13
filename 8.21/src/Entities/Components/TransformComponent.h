#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "Component.h"
#include "../src/Resources/Model.h"

#define TURN_CLOCKWISE 0
#define TURN_CCLOCKWISE 1

class Entity;
class FileReader;

struct ReadTransformFile {
	ReadTransformFile(FileReader& file, std::string_view section = "Transform");

	glm::vec3 _position = glm::vec3(0, 0, 0);
	glm::vec3 _scale = glm::vec3(1, 1, 1);
	glm::vec3 _rotation = glm::vec3(0, 0, 0);
	float _speed = 0.001f;
	bool _collidable = true;
};

class TransformComponent : public Component {
public:
	TransformComponent(std::shared_ptr<Entity> entity, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, float speed, bool collidable);
	TransformComponent(std::shared_ptr<Entity> new_entity, const TransformComponent& rhs);
	std::shared_ptr<Component> copy(std::shared_ptr<Entity> new_entity) const;

	void update();

	const int get_type() const;

	static constexpr int _type = TRANSFORM_COMPONENT;

	void save(std::ofstream& file);

	void move(glm::vec3 dir);
	void set(glm::vec3 pos);
	void set_direction(glm::vec3 dir);
	void set_destination(glm::vec3 dest);

	CollisionBox get_collision_box();

	Transform _transform;

	glm::vec3 _direction;
	glm::vec3 _destination;
	float _y_rot;
	int _turn;

	float _speed;

	bool _collidable;
	bool _dest_reached;

	CollisionBox _collision_box;

};

#endif