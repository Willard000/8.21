#ifndef COLLISION_H
#define COLLISION_H

#include <glm/gtc/matrix_transform.hpp>

struct CollisionBox {
	glm::vec3 min, max;
};

inline bool collision(CollisionBox a, CollisionBox b) {
	return !(a.max.x <= b.min.x || a.min.x >= b.max.x ||
			a.max.y <= b.min.y || a.min.y >= b.max.y ||
			a.max.z <= b.min.z || a.min.z >= b.max.z);
}

#endif