#include <octree.h>
#include <octree_test.h>
#include <volvis_utils/dataprovider.h>
#include <volvis_utils/reader.h>

int main() {
	vis::VolumeReader vr;
	vis::StructuredGridVolume* sg;
	sg = vr.ReadStructuredVolume("../../../data/raw/Bonsai.1.256x256x256.raw");
	AABB volumeBox = AABB(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(255.0f, 255.0f, 255.0f));
	OctreeNode tree = OctreeNode(volumeBox);
	BuildOctree(&tree, sg, 4, 0);
	printEmpty(&tree);
}

void printEmpty(OctreeNode* tree) {

	for (size_t i = 0; i < 8; i++) {

		std::cout << tree->isEmpty << std::endl;

		if (tree->isLeaf == false) return;

		printEmpty(tree->children[i]);
	}
}