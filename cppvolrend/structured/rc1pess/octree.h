#ifndef OCTREE_H
#define OCTREE_H

#include <algorithm>
#include <glm/glm.hpp>
#include <stdlib.h>
#include <volvis_utils/utils.h>

// Axis-Aligned Bounding Box
struct AABB {
	glm::vec3 min; // Min corner of box
	glm::vec3 max; // Max corner of box

	// Constructor
	AABB(const glm::vec3& minCorner, const glm::vec3& maxCorner);

	// Checks if point is inside AABB
	bool contains(const glm::vec3& point) const;

	// Computer center of AABB
	glm::vec3 center() const;

	// Get size of AABB
	glm::vec3 size() const;
};

// Octree
struct OctreeNode {
	bool isLeaf;             // True if node has no children
	bool isEmpty;            // True if node does not contain relevant data
	AABB bounds;             // Axis-Aligned Bounding Box (container for points)
	OctreeNode* children[8]; // Pointers to children

	// Constructor
	OctreeNode(const AABB& box);
};

// Recursively build the octree
void BuildOctree(OctreeNode* node, vis::StructuredGridVolume* volume, int maxDepth, int currentDepth);

// Compute the child bounds based on its index
AABB computeChildBounds(const AABB& parentBounds, int childIndex);

bool checkIfEmpty(vis::StructuredGridVolume* volume, const AABB& bounds);

glm::vec3 computeOffset(int childIndex, glm::vec3 size);

#endif // OCTREE_H