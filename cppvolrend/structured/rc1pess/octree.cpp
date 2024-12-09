#include "octree.h"

AABB computeChildBounds(const AABB& parentBounds, int childIndex);
bool checkIfEmpty(vis::StructuredGridVolume* volume, const AABB& bounds);
glm::vec3 computeOffset(int childIndex, glm::vec3 size);

AABB::AABB(const glm::vec3& minCorner, const glm::vec3& maxCorner) : min(minCorner), max(maxCorner) {}

bool AABB::contains(const glm::vec3& point) const {
	return (point.x >= min.x && point.x <= max.x &&
	        point.y >= min.y && point.y <= max.y &&
	        point.z >= min.z && point.z <= max.z);
}

glm::vec3 AABB::center() const {
	return (min + max) * 0.5f;
}

glm::vec3 AABB::size() const {
	return max - min;
}

OctreeNode::OctreeNode(const AABB& box) : isLeaf(true), isEmpty(false), bounds(box) {
	std::fill(std::begin(children), std::end(children), nullptr); // Set all pointers to nullptr
}

void BuildOctree(OctreeNode* node, vis::StructuredGridVolume* volume, int maxDepth, int currentDepth) {

	// Base case, max depth reached or empty node (should we keep constructing tree if a node is empty, i.e. do we want a balanced tree?)
	if (currentDepth >= maxDepth || node->isEmpty) {
		node->isLeaf = true;
		return;
	}
	currentDepth = currentDepth + 1;

	// Subdivide node
	node->isLeaf = false;
	for (size_t i = 0; i < 8; i++) {
		// Compute bounds for child node 'i'
		AABB childBounds = computeChildBounds(node->bounds, i);

		// Create child node
		node->children[i] = new OctreeNode(childBounds);
		
		// // Print child bounds
		// std::cout << "Bounds for child " << i << ":" << std::endl;
		// std::cout << "Min(x,y,z) = " << "(" << childBounds.min.x << "," << childBounds.min.y << "," << childBounds.min.z << ")" << std::endl;
		// std::cout << "Max(x,y,z) = " << "(" << childBounds.max.x << "," << childBounds.max.y << "," << childBounds.max.z << ")" << std::endl;

		// Check if child node contains any data
		node->children[i]->isEmpty = checkIfEmpty(volume, childBounds);

		// Recursively build the child node
		BuildOctree(node->children[i], volume, maxDepth, currentDepth);
	}
}

// Compute the child bounds based on its index
AABB computeChildBounds(const AABB& parentBounds, int childIndex) {
	glm::vec3 childSize = (parentBounds.max - parentBounds.min + glm::vec3(1,1,1)) * 0.5f;
	glm::vec3 offset = computeOffset(childIndex, childSize);
	return AABB(parentBounds.min + offset, parentBounds.min + offset + childSize - glm::vec3(1,1,1));
}

// Check if node is empty
bool checkIfEmpty(vis::StructuredGridVolume* volume, const AABB& bounds) {
	for (size_t x = 0; x < bounds.max.x; x++) {
		for (size_t y = 0; y < bounds.max.y; y++) {
			for (size_t z = 0; z < bounds.max.z; z++) {
				// Non-empty if there exists at least one sample that exceeds threshold (arbitrary value)
				if (volume->GetNormalizedSample(x, y, z) > 0.05) return false;
				// if (volume->GetAbsoluteSample(x, y, z) != 0 && volume->GetAbsoluteSample(x, y, z) != 35) return false;

			}
		}
	}
	return true;
}

// Calculate child axis offsets based on child index, using bit shifting
glm::vec3 computeOffset(int childIndex, glm::vec3 size) {
	float xOffset = (childIndex & 1) ? size.x : 0.0f;
	float yOffset = (childIndex & 2) ? size.y : 0.0f;
	float zOffset = (childIndex & 4) ? size.z : 0.0f;

	return glm::vec3(xOffset, yOffset, zOffset);
}

void FlattenOctree(OctreeNode* node, std::vector<GPUOctreeNode>& flatTree, int parentIndex) {
	GPUOctreeNode gpuNode;
	gpuNode.minBounds = node->bounds.min;
	gpuNode.maxBounds = node->bounds.max;
	gpuNode.isLeaf = node->isLeaf? 1 : 0;
	gpuNode.isEmpty = node->isEmpty? 1 : 0;

	if (!node->isLeaf) {
		for (int i = 0; i < 8; i++) {
			gpuNode.childIndices[i] = flatTree.size() + 1 + i;
			FlattenOctree(node->children[i], flatTree, flatTree.size());
		}
	} else {
		std::fill(std::begin(gpuNode.childIndices), std::end(gpuNode.childIndices), -1);
	}

	flatTree.push_back(gpuNode);
}