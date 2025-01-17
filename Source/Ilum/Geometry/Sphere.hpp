#pragma once

#include <glm/glm.hpp>

#include <vector>

namespace Ilum::geometry
{
struct Sphere
{
	glm::vec3 center = glm::vec3(0.f);
	float     radius = std::numeric_limits<float>::infinity();

	Sphere() = default;

	Sphere(const glm::vec3 &center, float radius);

	~Sphere() = default;

	void merge(const glm::vec3 &point);

	void merge(const std::vector<glm::vec3> &points);
};
}        // namespace Ilum::geometry