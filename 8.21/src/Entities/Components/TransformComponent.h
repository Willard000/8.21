#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "Component.h"
#include "../src/Resources/Model.h"

#define TURN_CLOCKWISE 0
#define TURN_CCLOCKWISE 1

class Entity;
class FileReader;

struct ReadTransformFile {
	ReadTransformFile(FileReader& file);

	glm::vec3 _scale = glm::vec3(0, 0, 0);
	glm::vec3 _rotation = glm::vec3(0, 0, 0);
	float _speed = 0.1f;
	bool _collidable = true;
};

class TransformComponent : public Component {
public:
	TransformComponent(std::shared_ptr<Entity> entity, glm::vec3 scale, glm::vec3 rotation, float speed, bool collidable);
	TransformComponent(std::shared_ptr<Entity> new_entity, const TransformComponent& rhs);
	std::shared_ptr<Component> copy(std::shared_ptr<Entity> new_entity) const;

	void update();

	const int get_type() const;

	static constexpr int _type = TRANSFORM_COMPONENT;

	void move(glm::vec3 dir);
	void set(const glm::vec3 pos);
	void set_direction(const glm::vec3 dir);

	Transform _transform;

	glm::vec3 _direction;
	float _y_rot;
	int _turn;

	float _speed;

	bool _collidable;

};

#endif