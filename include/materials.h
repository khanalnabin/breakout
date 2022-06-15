#ifndef MATERIALS_H_
#define MATERIALS_H_

#include <glm/glm.hpp>
#include <unordered_map>
#include <string>

struct material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

#endif // MATERIALS_H_
