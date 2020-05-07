#pragma once
#include "DirectedGraphConnection.h"
#include "SimDefines.h"
#include "ofFileUtils.h"

#include "bullet/btBulletCollisionCommon.h"
#include <vector>

class GraphNode
{
public:
	struct PrimitiveInfo {
		uint32_t index = 0;
		uint32_t primitiveType = PrimitiveType_Box;
		uint32_t jointType = JointType_Hinge;
		uint32_t recursionLimit = 1;
		btVector3 dimensions = btVector3(1.0, 1.0, 1.0);
		btVector3 parentAttachmentPlane = btVector3(1.0, 0.0, 0.0);

		PrimitiveInfo() {}
		PrimitiveInfo(uint32_t primitiveType, uint32_t recursionLim, btVector3 dims) :
			primitiveType(primitiveType), recursionLimit(recursionLim), dimensions(dims) {}
	};

	GraphNode();
	GraphNode(PrimitiveInfo info, bool isRoot);
	GraphNode(const GraphNode& g);

	void addConnection(GraphNode* child, const GraphConnection::JointInfo& info);

	uint32_t getRecursionLimit();
	uint32_t getGraphIndex();
	void setGraphIndex(uint32_t index);

	bool IsRootNode();

	PrimitiveInfo primitiveInfo;
	void save(std::string path);
	void load(ofFile file);

	std::vector<GraphConnection*> conns;

	static constexpr btScalar minSize = 0.125;
	static constexpr btScalar maxSize = 1.5;
	
private:
	bool _bIsRootNode = false;
};
