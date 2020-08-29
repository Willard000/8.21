#ifndef COMPONENT_H
#define COMPONENT_H

#include <memory>

enum
{
	TRANSFORM_COMPONENT,
	TOTAL_COMPONENTS
};

class Entity;

class Component {
public:
	Component(std::shared_ptr<Entity> entity) :
		_entity		(entity)
	{}
	~Component() {}

	virtual std::shared_ptr<Component> copy(std::shared_ptr<Entity> new_entity) const = 0;

	virtual void update() = 0;

	virtual const int get_type() const = 0;

	std::shared_ptr<Entity> _entity;
};

#endif