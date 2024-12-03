#include <algorithm>
#include <glm/glm.hpp>
#include <octree.h>
#include <stdlib.h>
#include <volvis_utils/utils.h>

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

// Recursively build the octree
void BuildOctree(OctreeNode* node, vis::StructuredGridVolume* volume, int maxDepth, int currentDepth) {
	if (currentDepth >= maxDepth || node->isEmpty) {
		node->isLeaf = true;
		return;
	}

	// Subdivide node
	node->isLeaf = false;

	for (size_t i = 0; i < 8; i++) {
		// Compute bounds for child node 'i'
		AABB childBounds = computeChildBounds(node->bounds, i);

		// Create child node
		node->children[i] = new OctreeNode(childBounds);

		// Check if child node contains any data
		node->children[i]->isEmpty = checkIfEmpty(volume, childBounds);

		// Recursively build the child node
		BuildOctree(node->children[i], volume, maxDepth, ++currentDepth);
	}
}

// Compute the child bounds based on its index
AABB computeChildBounds(const AABB& parentBounds, int childIndex) {
	glm::vec3 childSize = (parentBounds.max - parentBounds.min) * 0.5f;
	glm::vec3 offset = computeOffset(childIndex, childSize);
	return AABB(parentBounds.min + offset, parentBounds.min + offset + childSize);
}

bool checkIfEmpty(vis::StructuredGridVolume* volume, const AABB& bounds) {
	for (size_t x = 0; x < bounds.max.x; x++) {
		for (size_t y = 0; y < bounds.max.y; y++) {
			for (size_t z = 0; z < bounds.max.z; z++) {
				if (volume->GetNormalizedSample(x, y, z) > 0) return false;
			}
		}
	}
	return true;
}

glm::vec3 computeOffset(int childIndex, glm::vec3 size) {
	// Calculate offsets based on child index, using bit shifting
	float xOffset = (childIndex & 1) ? size.x * 0.5f : 0.0f;
	float yOffset = (childIndex & 2) ? size.y * 0.5f : 0.0f;
	float zOffset = (childIndex & 4) ? size.z * 0.5f : 0.0f;

	return glm::vec3(xOffset, yOffset, zOffset);
}