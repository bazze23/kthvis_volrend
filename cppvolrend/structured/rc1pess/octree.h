#ifndef OCTREE_H
#define OCTREE_H

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
	AABB bounds;             // Bounds in 3D space for this node
	OctreeNode* children[8]; // Pointers to children

	// Constructor
	OctreeNode(const AABB& box);
};

// Recursively build the octree based on a volume
void BuildOctree(OctreeNode* node, vis::StructuredGridVolume* volume, int maxDepth, int currentDepth);

struct GPUOctreeNode {
	glm::vec3 minBounds;
	glm::vec3 maxBounds;
	int childIndices[8]; // -1 if no child
	int isLeaf;
	int isEmpty;
};

void FlattenOctree(OctreeNode* node, std::vector<GPUOctreeNode>& flatTree, int parentIndex = -1);

#endif // OCTREE_H