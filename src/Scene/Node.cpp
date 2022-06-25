#include "Node.h"

Node::Node(std::string_view tagNode)
	: tag(tagNode)
{}

void Node::addChildren(std::shared_ptr<Node> childNode)
{
	childNode->setParent(shared_from_this());
	children.push_back(childNode);
}

void Node::setParent(std::weak_ptr<Node> parentNode)
{
	parent = parent;
}