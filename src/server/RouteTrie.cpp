#include "server/RouteTrie.hpp"

// ----------------------
// RouteTrieNode methods
// ----------------------

RouteTrieNode::RouteTrieNode() : isEnd(false), location(NULL) {}

// Helper: deep copy children from another node.
void RouteTrieNode::copyChildren(const RouteTrieNode &other)
{
	for (std::map<char, RouteTrieNode *>::const_iterator it = other.children.begin();
		 it != other.children.end(); ++it)
	{
		// Deep copy each child node
		children[it->first] = new RouteTrieNode(*(it->second));
	}
}

RouteTrieNode::RouteTrieNode(const RouteTrieNode &other)
	: isEnd(other.isEnd), location(other.location) // shallow copy of location pointer
{
	copyChildren(other);
}

RouteTrieNode &RouteTrieNode::operator=(const RouteTrieNode &other)
{
	if (this != &other)
	{
		// First, delete current children
		for (std::map<char, RouteTrieNode *>::iterator it = children.begin();
			 it != children.end(); ++it)
		{
			delete it->second;
		}
		children.clear();

		isEnd = other.isEnd;
		location = other.location; // shallow copy

		copyChildren(other);
	}
	return *this;
}

RouteTrieNode::~RouteTrieNode()
{
	for (std::map<char, RouteTrieNode *>::iterator it = children.begin();
		 it != children.end(); ++it)
	{
		delete it->second;
	}
	children.clear();
}

// ----------------------
// RouteTrie methods
// ----------------------

RouteTrie::RouteTrie()
{
	root = new RouteTrieNode();
}

// Helper: deep copy a node recursively.
RouteTrieNode *RouteTrie::copyNode(const RouteTrieNode *node)
{
	if (!node)
		return NULL;
	RouteTrieNode *newNode = new RouteTrieNode(*node);
	return newNode;
}

RouteTrie::RouteTrie(const RouteTrie &other)
{
	root = copyNode(other.root);
}

RouteTrie &RouteTrie::operator=(const RouteTrie &other)
{
	if (this != &other)
	{
		delete root;
		root = copyNode(other.root);
	}
	return *this;
}

RouteTrie::~RouteTrie()
{
	delete root;
}

// Insert a Route pointer into the trie using its URI path.
void RouteTrie::insert(Route *loc)
{
	if (!loc)
		return;
	std::string path = loc->getPath();
	RouteTrieNode *node = root;
	for (size_t i = 0; i < path.size(); ++i)
	{
		char c = path[i];
		if (node->children.find(c) == node->children.end())
		{
			node->children[c] = new RouteTrieNode();
		}
		node = node->children[c];
	}
	node->isEnd = true;
	node->location = loc;
}

// Given a URI, traverse the trie to find the longest matching location prefix.
// Returns the pointer to the Route if found; otherwise, returns NULL.
Route *RouteTrie::searchLongestPrefix(const std::string &uri) const
{
	RouteTrieNode *node = root;
	Route *lastFound = NULL;
	for (size_t i = 0; i < uri.size(); ++i)
	{
		char c = uri[i];
		if (node->children.find(c) == node->children.end())
			break;
		node = node->children[c];
		if (node->isEnd)
			lastFound = node->location;
	}
	return lastFound;
}

void RouteTrie::collectLocations(const RouteTrieNode *node, std::vector<Route *> &locations) const
{
	if (!node)
		return;
	if (node->isEnd && node->location)
		locations.push_back(node->location);
	for (std::map<char, RouteTrieNode *>::const_iterator it = node->children.begin();
		 it != node->children.end(); ++it)
	{
		collectLocations(it->second, locations);
	}
}

std::vector<Route *> RouteTrie::getAllLocations() const
{
	std::vector<Route *> locations;
	collectLocations(root, locations);
	return locations;
}
