#include "octree.h"

AABB computeChildBounds(const AABB& parentBounds, int childIndex);
void setMinMaxVal(vis::StructuredGridVolume* volume, OctreeNode* node, const AABB& bounds);
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

OctreeNode::OctreeNode(const AABB& box) : minVal(0), maxVal(0), isLeaf(true), bounds(box) {
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

		// Compute bounds for child node 'i'
		AABB childBounds = computeChildBounds(node->bounds, i);

		// Create child node
		node->children[i] = new OctreeNode(childBounds);

		// Set min and max values in node
		setMinMaxVal(volume, node->children[i], childBounds);

		// Recursively build the child node
		BuildOctree(node->children[i], volume, maxDepth, currentDepth);
	}

	// Set min/max values for root node
	setMinMaxVal(volume, node, node->bounds);
}

// Compute the child bounds based on its index
AABB computeChildBounds(const AABB& parentBounds, int childIndex) {
	glm::vec3 childSize = (parentBounds.max - parentBounds.min + glm::vec3(1, 1, 1)) * 0.5f;
	glm::vec3 offset = computeOffset(childIndex, childSize);
	return AABB(parentBounds.min + offset, parentBounds.min + offset + childSize - glm::vec3(1, 1, 1));
}

// Calculate child axis offsets based on child index, using bit shifting
glm::vec3 computeOffset(int childIndex, glm::vec3 size) {
	float xOffset = (childIndex & 1) ? size.x : 0.0f;
	float yOffset = (childIndex & 2) ? size.y : 0.0f;
	float zOffset = (childIndex & 4) ? size.z : 0.0f;

	return glm::vec3(xOffset, yOffset, zOffset);
}

// Find and set min/max values for a node
void setMinMaxVal(vis::StructuredGridVolume* volume, OctreeNode* node, const AABB& bounds) {
	float min = 10;
	float max = -10;
	for (size_t x = bounds.min.x; x < bounds.max.x; x++) {
		for (size_t y = bounds.min.y; y < bounds.max.y; y++) {
			for (size_t z = bounds.min.z; z < bounds.max.z; z++) {
				float sample = volume->GetNormalizedSample(x, y, z);
				if (sample < min) min = sample;
				if (sample > max) max = sample;
			}
		}
	}
	node->minVal = min;
	node->maxVal = max;
}


void FlattenOctree(OctreeNode* node, std::vector<GPUOctreeNode>& flatTree) {
    // Create a GPUOctreeNode to store the current node's data
    GPUOctreeNode gpuNode;
    gpuNode.minBounds = node->bounds.min;
    gpuNode.maxBounds = node->bounds.max;
    gpuNode.minVal = node->minVal;
    gpuNode.maxVal = node->maxVal;
    gpuNode.isLeaf = node->isLeaf ? 1 : 0;

    // Add the node to the flatTree
    flatTree.push_back(gpuNode);
    
    // Get the index of the current node in flatTree (newest element)
    int currentNodeIndex = flatTree.size() - 1;

    if (!node->isLeaf) {
        // For internal nodes, update the child indices first
        // All 8 possible children of an internal node
        for (int i = 0; i < 8; i++) {
            // Set the index of this child
            flatTree[currentNodeIndex].childIndices[i] = flatTree.size();

            // Recurse into the child and flatten it
            FlattenOctree(node->children[i], flatTree);  // Recursive call for the child node
        }
    } else {
        // For leaf nodes, set all child indices to -1 (as they have no children)
        std::fill(std::begin(flatTree[currentNodeIndex].childIndices), 
                  std::end(flatTree[currentNodeIndex].childIndices), 
                  -1);
    }
}