#ifndef NODE_H
#define NODE_H

#include "vector"
#include <memory>
#include <string>
//#include <GLFW/glfw3.h>
//#include "gl_core_4_4.h"
//#include <string_view>

class Node: public std::enable_shared_from_this<Node>
{
public:
	Node(std::string_view tagNode);
	void addChildren(std::shared_ptr<Node> childNode);
	void setParent(std::weak_ptr<Node> parentNode);
private:
	std::weak_ptr<Node> parent;
	std::vector<std::shared_ptr<Node>> children;
	std::string tag;
};

#endif