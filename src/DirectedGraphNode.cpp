#include "DirectedGraphNode.h"
#include "DirectedGraphConnection.h"
#include <iomanip>

#include "nlohmann/json.hpp"

using json = nlohmann::json;
void from_json(const json& j, GraphNode::PrimitiveInfo& p);
void to_json(json& j, const GraphNode::PrimitiveInfo& p);

GraphNode::GraphNode() {}

GraphNode::GraphNode(PrimitiveInfo info, bool isRoot) :
	primitiveInfo(info), _bIsRootNode(isRoot) {}

GraphNode::GraphNode(const GraphNode& g) :
	primitiveInfo(g.primitiveInfo), _bIsRootNode(g._bIsRootNode) {}

void GraphNode::addConnection(GraphNode* child, const GraphConnection::JointInfo& info) {
	conns.push_back(new GraphConnection(this, child, info));
}

uint32_t GraphNode::getRecursionLimit() {
	return primitiveInfo.recursionLimit;
}

uint32_t GraphNode::getGraphIndex()
{
	return primitiveInfo.index;
}

void GraphNode::setGraphIndex(uint32_t index)
{
	primitiveInfo.index = index;
}

void GraphNode::setIsRootNode(bool bIsRoot)
{
	_bIsRootNode = bIsRoot;
}

bool GraphNode::IsRootNode() 
{
	return _bIsRootNode;
}

void GraphNode::save(std::string path)
{
	json j = primitiveInfo;
	
	ofFile f(path, ofFile::WriteOnly, false);
	f << std::setw(4) << j << std::endl;
	f.close();
}

void GraphNode::load(ofFile& file)
{
	json j;
	file >> j;
	GraphNode::PrimitiveInfo fromFile = j.get<GraphNode::PrimitiveInfo>();
	primitiveInfo = fromFile;
	bool bIsRoot = fromFile.index == 0; // todo: probably works but need to fix later
}

GraphNode::~GraphNode() 
{
	for (GraphConnection* c : conns) {
		delete c;
	}
}

void to_json(json& j, const GraphNode::PrimitiveInfo& p)
{
	j = json{
		{"index", p.index},
		{"primitiveType", p.primitiveType},
		{"jointType", p.jointType},
		{"recursionLimit", p.recursionLimit},
		{"dimensions_x", double(p.dimensions.x())},
		{"dimensions_y", double(p.dimensions.y())},
		{"dimensions_z", double(p.dimensions.z())},
		{"parentAttachmentPlane_x", double(p.parentAttachmentPlane.x())},
		{"parentAttachmentPlane_y", double(p.parentAttachmentPlane.y())},
		{"parentAttachmentPlane_z", double(p.parentAttachmentPlane.z())},
	};
}

void from_json(const json& j, GraphNode::PrimitiveInfo& p)
{
	j.at("index").get_to(p.index);
	j.at("primitiveType").get_to(p.primitiveType);
	j.at("jointType").get_to(p.jointType);
	j.at("recursionLimit").get_to(p.recursionLimit);

	double x, y, z;
	j.at("dimensions_x").get_to(x);
	j.at("dimensions_x").get_to(y);
	j.at("dimensions_x").get_to(z);
	p.dimensions = btVector3(x, y, z);

	j.at("parentAttachmentPlane_x").get_to(x);
	j.at("parentAttachmentPlane_x").get_to(y);
	j.at("parentAttachmentPlane_x").get_to(z);
	p.parentAttachmentPlane = btVector3(x, y, z);
}
