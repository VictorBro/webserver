#pragma once

#include <string>
#include <map>
#include "server/Route.hpp"

class RouteTrieNode
{
public:
	RouteTrieNode();
	RouteTrieNode(const RouteTrieNode &other);
	RouteTrieNode &operator=(const RouteTrieNode &other);
	~RouteTrieNode();

	// Children nodes: key is a character, value is a pointer to a child node.
	std::map<char, RouteTrieNode *> children;
	bool isEnd; // True if this node marks the end of a valid location path
	Route *location;

private:
	// Helper function to deep copy children nodes
	void copyChildren(const RouteTrieNode &other);
};

class RouteTrie
{
public:
	RouteTrie();
	// Copy constructor (deep copy of the trie)
	RouteTrie(const RouteTrie &other);
	RouteTrie &operator=(const RouteTrie &other);
	~RouteTrie();

	void insert(Route *loc);

	// Given a URI, return the Route pointer with the longest matching prefix.
	// Returns NULL if no match is found.
	Route *searchLongestPrefix(const std::string &uri) const;
	std::vector<Route *> getAllLocations() const;

private:
	RouteTrieNode *root;

	// Helper function to deep copy a trie node
	static RouteTrieNode *copyNode(const RouteTrieNode *node);
	void collectLocations(const RouteTrieNode *node, std::vector<Route *> &locations) const;
};
