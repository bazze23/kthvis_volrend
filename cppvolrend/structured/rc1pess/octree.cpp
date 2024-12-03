#include <algorithm>
#include <glm/glm.hpp>
#include <stdlib.h>

// Octree
struct OctreeNode {
	bool isLeaf;             // True if node has no children
	bool isEmpty;            // True if node does not contain relevant data
	AABB bounds;             // Axis-Aligned Bounding Box (container for points)
	OctreeNode* children[8]; // Pointers to children

	// Constructor
	OctreeNode(const AABB& box) : isLeaf(true), isEmpty(true), bounds(box) {
		std::fill(std::begin(children), std::end(children), nullptr); // Set all pointers to nullptr
	}
};

// Axis-Aligned Bounding Box
struct AABB {
	glm::vec3 min; // Min corner of box
	glm::vec3 max; // Max corner of box

	// Constructor
	AABB(const glm::vec3& minCorner, const glm::vec3& maxCorner) : min(minCorner), max(maxCorner) {}

	// Checks if point is inside AABB
	bool contains(const glm::vec3& point) const {
		return (point.x >= min.x && point.x <= max.x &&
		        point.y >= min.y && point.y <= max.y &&
		        point.z >= min.z && point.z <= max.z);
	}

	// Computer center of AABB
	glm::vec3 center() const {
		return (min + max) * 0.5f;
	}

	// Get size of AABB
	glm::vec3 size() const {
		return max - min;
	}
};
