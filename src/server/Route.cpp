#include "server/Route.hpp"
#include "utils/Consts.hpp"

Route::Route() : _path(""),
					   _allowedMethods(kDefaultAllowedMethods),
					   _root(kDefaultRoot),
					   _index(kDefaultIndex.begin(), kDefaultIndex.end()),
					   _autoindex(kDefaultAutoindex),
					   _returnDirective(),
					   _uploadDirectory(""),
					   // Initialize all flags to false
					   _allowedMethodsSet(false),
					   _rootSet(false),
					   _indexSet(false),
					   _autoindexSet(false),
					   _returnDirectiveSet(false),
					   _uploadDirectorySet(false)
{
}

Route::Route(const std::string &path) : _path(path),
											  _allowedMethods(kDefaultAllowedMethods),
											  _root(kDefaultRoot),
											  _index(kDefaultIndex.begin(), kDefaultIndex.end()),
											  _autoindex(kDefaultAutoindex),
											  _returnDirective(),
											  _uploadDirectory(""),
											  // Initialize all flags to false
											  _allowedMethodsSet(false),
											  _rootSet(false),
											  _indexSet(false),
											  _autoindexSet(false),
											  _returnDirectiveSet(false),
											  _uploadDirectorySet(false)
{
}

Route::Route(const Route &other) : _path(other._path),
											_allowedMethods(other._allowedMethods),
											_root(other._root),
											_index(other._index),
											_autoindex(other._autoindex),
											_returnDirective(other._returnDirective),
											_uploadDirectory(other._uploadDirectory),
											// Copy all "isSet" flags
											_allowedMethodsSet(other._allowedMethodsSet),
											_rootSet(other._rootSet),
											_indexSet(other._indexSet),
											_autoindexSet(other._autoindexSet),
											_returnDirectiveSet(other._returnDirectiveSet),
											_uploadDirectorySet(other._uploadDirectorySet)
{
}

Route &Route::operator=(const Route &other)
{
	if (this != &other)
	{
		_path = other._path;
		_allowedMethods = other._allowedMethods;
		_root = other._root;
		_index = other._index;
		_autoindex = other._autoindex;
		_returnDirective = other._returnDirective;
		_uploadDirectory = other._uploadDirectory;
		// Copy all "isSet" flags
		_allowedMethodsSet = other._allowedMethodsSet;
		_rootSet = other._rootSet;
		_indexSet = other._indexSet;
		_autoindexSet = other._autoindexSet;
		_returnDirectiveSet = other._returnDirectiveSet;
		_uploadDirectorySet = other._uploadDirectorySet;
	}
	return *this;
}

Route::~Route()
{
	// Nothing to free explicitly.
}

void Route::setPath(const std::string &path)
{
	_path = path;
}

const std::string &Route::getPath() const
{
	return _path;
}

void Route::addAllowedMethod(const std::string &method)
{
	if (!_allowedMethodsSet)
	{
		_allowedMethods.clear();
	}
	_allowedMethods[method] = true;
	_allowedMethodsSet = true;
}

const std::map<std::string, bool> &Route::getAllowedMethods() const
{
	return _allowedMethods;
}

bool Route::isAllowedMethodSet() const
{
	return _allowedMethodsSet;
}

void Route::setRoot(const std::string &root)
{
	_root = root;
	_rootSet = true;
}

const std::string &Route::getRoot() const
{
	return _root;
}

bool Route::isRootSet() const
{
	return _rootSet;
}

void Route::addIndex(const std::string &indexFile)
{
	if (!_indexSet)
	{
		_index.clear();
	}
	_index.insert(indexFile);
	_indexSet = true;
}

const std::set<std::string> &Route::getIndex() const
{
	return _index;
}

bool Route::isIndexSet() const
{
	return _indexSet;
}

void Route::setAutoindex(bool autoindex)
{
	_autoindex = autoindex;
	_autoindexSet = true;
}

bool Route::getAutoindex() const
{
	return _autoindex;
}

bool Route::isAutoindexSet() const
{
	return _autoindexSet;
}

void Route::setReturnDirective(const std::string &statusCode, const std::string &ret)
{
	if (!_returnDirectiveSet)
	{
		_returnDirective.first = statusCode;
		_returnDirective.second = ret;
		_returnDirectiveSet = true;
	}
}

const std::pair<std::string, std::string> &Route::getReturnDirective() const
{
	return _returnDirective;
}

bool Route::isReturnDirectiveSet() const
{
	return _returnDirectiveSet;
}

void Route::setUploadDirectory(const std::string &uploadDir)
{
	_uploadDirectory = uploadDir;
	_uploadDirectorySet = true;
}

const std::string &Route::getUploadDirectory() const
{
	return _uploadDirectory;
}

bool Route::isUploadDirectorySet() const
{
	return _uploadDirectorySet;
}
