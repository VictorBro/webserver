#include <cstdlib>
#include "server/VirtualHost.hpp"
#include "utils/Consts.hpp"
#include "utils/StringUtils.hpp"

VirtualHost::VirtualHost() : _listens(),
				   _serverNames(),
				   _root(kDefaultRoot),
				   _index(kDefaultIndex.begin(), kDefaultIndex.end()),
				   _errorPages(kDefaultErrorPages),
				   _allowedMethods(kDefaultAllowedMethods),
				   _autoindex(kDefaultAutoindex),
				   _cgiBin(),
				   _returnDirective(),
				   _locationTrie(),
				   // Initialize all "isSet" flags to false
				   _listensSet(false),
				   _serverNamesSet(false),
				   _rootSet(false),
				   _indexSet(false),
				   _errorPagesSet(),
				   _allowedMethodsSet(false),
				   _autoindexSet(false),
				   _returnDirectiveSet(false)
{
	_listens.insert(kDefaultListen);
	_serverNames.insert(kDefaultServerName);
}

VirtualHost::VirtualHost(const VirtualHost &other) : _listens(other._listens),
									  _serverNames(other._serverNames),
									  _root(other._root),
									  _index(other._index),
									  _errorPages(other._errorPages),
									  _allowedMethods(other._allowedMethods),
									  _autoindex(other._autoindex),
									  _cgiBin(other._cgiBin),
									  _returnDirective(other._returnDirective),
									  _locationTrie(other._locationTrie),
									  // Copy all "isSet" flags
									  _listensSet(other._listensSet),
									  _serverNamesSet(other._serverNamesSet),
									  _rootSet(other._rootSet),
									  _indexSet(other._indexSet),
									  _errorPagesSet(other._errorPagesSet),
									  _allowedMethodsSet(other._allowedMethodsSet),
									  _autoindexSet(other._autoindexSet),
									  _returnDirectiveSet(other._returnDirectiveSet)
{
}

VirtualHost &VirtualHost::operator=(const VirtualHost &other)
{
	if (this != &other)
	{
		_listens = other._listens;
		_serverNames = other._serverNames;
		_root = other._root;
		_index = other._index;
		_errorPages = other._errorPages;
		_allowedMethods = other._allowedMethods;
		_autoindex = other._autoindex;
		_cgiBin = other._cgiBin;
		_returnDirective = other._returnDirective;
		_locationTrie = other._locationTrie;
		// Copy all "isSet" flags
		_listensSet = other._listensSet;
		_serverNamesSet = other._serverNamesSet;
		_rootSet = other._rootSet;
		_indexSet = other._indexSet;
		_errorPagesSet = other._errorPagesSet;
		_allowedMethodsSet = other._allowedMethodsSet;
		_autoindexSet = other._autoindexSet;
		_returnDirectiveSet = other._returnDirectiveSet;
	}
	return *this;
}

VirtualHost::~VirtualHost()
{
	std::vector<Route *> locations = _locationTrie.getAllLocations();
	for (std::vector<Route *>::iterator it = locations.begin();
		 it != locations.end(); ++it)
	{
		delete *it;
	}
}

void VirtualHost::addListen(const std::string &listen)
{
	if (!_listensSet)
	{
		_listens.clear();
	}
	_listens.insert(listen);
	_listensSet = true;
}

const std::set<std::string> &VirtualHost::getListens() const
{
	return _listens;
}

bool VirtualHost::isListenSet(const std::string &listen) const
{
	if (!_listensSet)
		return false;
	return _listens.find(listen) != _listens.end();
}

bool VirtualHost::isListensSet() const
{
	return _listensSet;
}

void VirtualHost::addServerName(const std::string &name)
{
	_serverNames.insert(name);
	_serverNamesSet = true;
}

const std::set<std::string> &VirtualHost::getServerNames() const
{
	return _serverNames;
}

bool VirtualHost::isServerNamesSet() const
{
	return _serverNamesSet;
}

void VirtualHost::setRoot(const std::string &root)
{
	_root = root;
	_rootSet = true;
}

const std::string &VirtualHost::getRoot() const
{
	return _root;
}

bool VirtualHost::isRootSet() const
{
	return _rootSet;
}

void VirtualHost::addIndex(const std::string &indexFile)
{
	if (!_indexSet)
	{
		_index.clear();
	}
	_index.insert(indexFile);
	_indexSet = true;
}

const std::set<std::string> &VirtualHost::getIndex() const
{
	return _index;
}

bool VirtualHost::isIndexSet() const
{
	return _indexSet;
}

void VirtualHost::addErrorPage(int code, const std::string &path)
{
	if (_errorPagesSet.find(code) == _errorPagesSet.end())
	{
		_errorPages[code] = path;
		_errorPagesSet.insert(code);
	}
}

const std::map<int, std::string> &VirtualHost::getErrorPages() const
{
	return _errorPages;
}

void VirtualHost::addAllowedMethod(const std::string &method)
{
	if (!_allowedMethodsSet)
	{
		_allowedMethods.clear();
	}
	_allowedMethods[method] = true;
	_allowedMethodsSet = true;
}

const std::map<std::string, bool> &VirtualHost::getAllowedMethods() const
{
	return _allowedMethods;
}

bool VirtualHost::isAllowedMethodsSet() const
{
	return _allowedMethodsSet;
}

void VirtualHost::setAutoindex(bool autoindex)
{
	_autoindex = autoindex;
	_autoindexSet = true;
}

bool VirtualHost::getAutoindex() const
{
	return _autoindex;
}

bool VirtualHost::isAutoindexSet() const
{
	return _autoindexSet;
}

void VirtualHost::addCgiBin(const std::string &ext, const std::string &cgiBin)
{
	_cgiBin[ext] = cgiBin;
}

const std::map<std::string, std::string> &VirtualHost::getCgiBin() const
{
	return _cgiBin;
}

void VirtualHost::setReturnDirective(const std::string &statusCode, const std::string &ret)
{
	if (!_returnDirectiveSet)
	{
		_returnDirective.first = statusCode;
		_returnDirective.second = ret;
		_returnDirectiveSet = true;
	}
}

const std::pair<std::string, std::string> &VirtualHost::getReturnDirective() const
{
	return _returnDirective;
}

bool VirtualHost::isReturnDirectiveSet() const
{
	return _returnDirectiveSet;
}

void VirtualHost::addLocation(Route *loc)
{
	if (loc)
	{
		_locationTrie.insert(loc);
	}
}

Route *VirtualHost::getLocationForURI(const std::string &uri) const
{
	return _locationTrie.searchLongestPrefix(uri);
}

std::vector<Route *> VirtualHost::getLocations() const
{
	return _locationTrie.getAllLocations();
}
