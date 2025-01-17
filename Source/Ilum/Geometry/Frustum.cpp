#include "Frustum.hpp"

namespace Ilum::geometry
{
Frustum::Frustum(const glm::mat4 &view_projection)
{
	glm::mat4 inverse = glm::inverse(view_projection);

	// Left
	planes[0].normal.x = view_projection[0].w + view_projection[0].x;
	planes[0].normal.y = view_projection[1].w + view_projection[1].x;
	planes[0].normal.z = view_projection[2].w + view_projection[2].x;
	planes[0].constant = view_projection[3].w + view_projection[3].x;

	// Right
	planes[1].normal.x = view_projection[0].w - view_projection[0].x;
	planes[1].normal.y = view_projection[1].w - view_projection[1].x;
	planes[1].normal.z = view_projection[2].w - view_projection[2].x;
	planes[1].constant = view_projection[3].w - view_projection[3].x;

	// Top
	planes[2].normal.x = view_projection[0].w - view_projection[0].y;
	planes[2].normal.y = view_projection[1].w - view_projection[1].y;
	planes[2].normal.z = view_projection[2].w - view_projection[2].y;
	planes[2].constant = view_projection[3].w - view_projection[3].y;

	// Bottom
	planes[3].normal.x = view_projection[0].w + view_projection[0].y;
	planes[3].normal.y = view_projection[1].w + view_projection[1].y;
	planes[3].normal.z = view_projection[2].w + view_projection[2].y;
	planes[3].constant = view_projection[3].w + view_projection[3].y;

	// Near
	planes[4].normal.x = view_projection[0].w + view_projection[0].z;
	planes[4].normal.y = view_projection[1].w + view_projection[1].z;
	planes[4].normal.z = view_projection[2].w + view_projection[2].z;
	planes[4].constant = view_projection[3].w + view_projection[3].z;

	// Far
	planes[5].normal.x = view_projection[0].w - view_projection[0].z;
	planes[5].normal.y = view_projection[1].w - view_projection[1].z;
	planes[5].normal.z = view_projection[2].w - view_projection[2].z;
	planes[5].constant = view_projection[3].w - view_projection[3].z;

	for (auto& plane : planes)
	{
		float length = glm::length(plane.normal);
		plane.normal /= length;
		plane.constant /= length;
	}
}
}        // namespace Ilum::geometry