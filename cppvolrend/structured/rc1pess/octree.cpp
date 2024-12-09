#include "octree.h"

AABB computeChildBounds(const AABB& parentBounds, int childIndex);
void setMinMaxVal(vis::StructuredGridVolume* volume, OctreeNode* node, const AABB& bounds);
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

OctreeNode::OctreeNode(const AABB& box) : minVal(0), maxVal(0), isLeaf(true), isEmpty(false), bounds(box) {
	std::fill(std::begin(children), std::end(children), nullptr); // Set all pointers to nullptr
}

void BuildOctree(OctreeNode* node, vis::StructuredGridVolume* volume, int maxDepth, int currentDepth) {

	// Base case, max depth reached
	if (currentDepth >= maxDepth) {
		node->isLeaf = true;
		return;
	}
	currentDepth = currentDepth + 1;

	// Subdivide node
	node->isLeaf = false;
	for (size_t i = 0; i < 8; i++) {
		node->isEmpty = false; // Set to false for now, may use later in shader

		// Compute bounds for child node 'i'
		AABB childBounds = computeChildBounds(node->bounds, i);

		// Create child node
		node->children[i] = new OctreeNode(childBounds);

		// // Print child bounds
		// std::cout << "Bounds for child " << i << ":" << std::endl;
		// std::cout << "Min(x,y,z) = " << "(" << childBounds.min.x << "," << childBounds.min.y << "," << childBounds.min.z << ")" << std::endl;
		// std::cout << "Max(x,y,z) = " << "(" << childBounds.max.x << "," << childBounds.max.y << "," << childBounds.max.z << ")" << std::endl;

		// Set min and max values in node
		setMinMaxVal(volume, node->children[i], childBounds);

		// Recursively build the child node
		BuildOctree(node->children[i], volume, maxDepth, currentDepth);
	}

	// Set min/max values for root node (is this necessary?)
	setMinMaxVal(volume, node, node->bounds);
}

// Compute the child bounds based on its index
AABB computeChildBounds(const AABB& parentBounds, int childIndex) {
	glm::vec3 childSize = (parentBounds.max - parentBounds.min + glm::vec3(1, 1, 1)) * 0.5f;
	glm::vec3 offset = computeOffset(childIndex, childSize);
	return AABB(parentBounds.min + offset, parentBounds.min + offset + childSize - glm::vec3(1, 1, 1));
}

// Find and set min/max values for a node
void setMinMaxVal(vis::StructuredGridVolume* volume, OctreeNode* node, const AABB& bounds) {
	float min = 1;
	float max = 0;
	for (size_t x = 0; x < bounds.max.x; x++) {
		for (size_t y = 0; y < bounds.max.y; y++) {
			for (size_t z = 0; z < bounds.max.z; z++) {
				float sample = volume->GetNormalizedSample(x, y, z);
				if (sample < min) min = sample;
				if (sample > max) max = sample;
			}
		}
	}
	node->minVal = min;
	node->maxVal = max;
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

void FlattenOctree(OctreeNode* node, GPUOctreeNode* gpuNode, std::vector<GPUOctreeNode>& flatTree) {
	gpuNode->minVal = node->minVal;
	gpuNode->maxVal = node->maxVal;
	gpuNode->minBounds = node->bounds.min;
	gpuNode->maxBounds = node->bounds.max;
	gpuNode->isLeaf = node->isLeaf ? 1 : 0;
	gpuNode->isEmpty = node->isEmpty ? 1 : 0;

	flatTree.push_back(*gpuNode);

	if (!node->isLeaf) {
		for (int i = 0; i < 8; i++) {
			GPUOctreeNode tmp;
			gpuNode->childIndices[i] = flatTree.size();
			FlattenOctree(node->children[i], &tmp, flatTree); // DFS, left to right
		}
	} else {
		std::fill(std::begin(gpuNode->childIndices), std::end(gpuNode->childIndices), -1);
	}
}