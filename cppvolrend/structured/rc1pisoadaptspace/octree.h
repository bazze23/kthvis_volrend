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
	float minVal;
	float maxVal;
	bool isLeaf;             // True if node has no children
	AABB bounds;             // Bounds in 3D space for this node
	OctreeNode* children[8]; // Pointers to children

	// Constructor
	OctreeNode(const AABB& box);
};

// Recursively build the octree based on a volume
void BuildOctree(OctreeNode* node, vis::StructuredGridVolume* volume, int maxDepth, int currentDepth);

struct alignas(16) GPUOctreeNode {
    glm::vec3 minBounds;   // 12 bytes
    float padding1;        // 4 bytes for alignment
    glm::vec3 maxBounds;   // 12 bytes
    float padding2;        // 4 bytes for alignment
    float minVal;          // 4 bytes
    float maxVal;          // 4 bytes
    int childIndices[8];   // 32 bytes
    int isLeaf;            // 4 bytes
};

void FlattenOctree(OctreeNode* node, GPUOctreeNode* gpuNode, std::vector<GPUOctreeNode>& flatTree);

#endif // OCTREE_H