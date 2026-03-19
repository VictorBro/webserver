#pragma once

#include <map>
#include <set>
#include <sys/epoll.h>
#include "utils/Consts.hpp"
#include "server/ServerKey.hpp"

class VirtualHost;
class Route;
class Connection;

const int kMaxEvents = 10;

enum ParseState
{
	GLOBAL,
	SERVER,
	LOCATION
};

class HttpServer
{
public:
	HttpServer(const std::string &filename);
	HttpServer(const HttpServer &ws);
	HttpServer &operator=(const HttpServer &ws);
	~HttpServer();

	void run();
	void printSettings() const;
	void closeExpiredConnections();

	// Register a CGI process ID for cleanup
	void registerCgiProcess(pid_t pid);

	// Setters / Getters
	void setClientTimeout(int timeout);
	int getClientTimeout() const;
	bool isClientTimeoutSet() const;

	// Convert a size string (e.g., "1k", "2m") to bytes.
	// possible suffixes: k, K, m, M or none (bytes)
	void setClientHeaderBufferSize(const std::string &size);
	int getClientHeaderBufferSize() const;
	bool isClientHeaderBufferSizeSet() const;

	// possible suffixes: k, K, m, M or none (bytes)
	void setClientMaxBodySize(const std::string &size);
	size_t getClientMaxBodySize() const;
	bool isClientMaxBodySizeSet() const;

	const std::map<ServerKey, VirtualHost *> &getServers() const;

private:
	std::string _fileName;
	int _epfd;
	std::map<int, std::pair<std::string, std::string> > _listeners; // key: file descriptor, value: pair of local host and port
	int _clientTimeout;											   // in seconds; Default: 75
	bool _clientTimeoutSet;
	int _clientHeaderBufferSize; // in bytes; Default: 2k
	bool _clientHeaderBufferSizeSet;
	size_t _clientMaxBodySize; // in bytes; Default: 1m
	bool _clientMaxBodySizeSet;
	struct epoll_event _evlist[kMaxEvents];
	std::map<ServerKey, VirtualHost *> _servers;
	std::map<int, Connection *> _connections; // key: file descriptor, value: Connection object
	std::map<int, Connection *> _pipes;	// key: file descriptor, value: Connection object
	std::set<int> _cgiPids;	// set of CGI process PIDs

	void parseConfig();
	void initEpoll();
	void cleanupEpoll();
	std::string readUntilDelimiter(std::istream &file, const std::string &delimiters);
	void handleOpenBracket(const std::string &content_block, VirtualHost *&curr_server, Route *&curr_location, ParseState &state);
	void handleDirective(const std::string &content_block, VirtualHost *curr_server, Route *curr_location, ParseState &state);
	void handleCloseBracket(const std::string &content_block, VirtualHost *&curr_server, Route *&curr_location, ParseState &state);
	void resolveAndAddListen(const std::string &listen, VirtualHost *server);
	void validateSizeFormat(const std::string &size);
	void handle_cgi_bin_directive(const std::vector<std::string> &words, VirtualHost *curr_server);
	void validateUrl(const std::string &url) const;
	void validateReturnDirective(const std::vector<std::string> &words);
	void validateRootDirective(const std::vector<std::string> &words);
	void handleGlobalDirective(const std::vector<std::string> &words);
	void handleServerDirective(const std::vector<std::string> &words, VirtualHost *curr_server);
	void handleLocationDirective(const std::vector<std::string> &words, Route *curr_location);
	void inheritServerDirectives(VirtualHost *curr_server);
	void addServer(VirtualHost *server);
	void processPollEvents(int ready);
	bool updateEpollEvents(int fd, uint32_t events);
	bool addEpollEvents(int fd, uint32_t events);
	void closeListenerSockets();
	void cleanupServers();
	void cleanupConnections();
	void cleanupPipes();
	void handleConnectionClose(int fd);
	void setupListenerSockets();
	void handleNewConnection(int listener);
	void handleClientRecv(int fd);
	void handleClientSend(int fd);
	void handleCgiRecv(int fd);
	void finalizeCgiRecv(int fd);
	void handleCgiSend(int fd);

	//methods for CGI process shutdown
	void terminateCgiProcesses(bool graceful = true);
	bool waitForCgiProcesses(int timeoutMs, int checkIntervalMs = 50);
	void reapDeadChildren(int timeoutMs = 200, int checkIntervalMs = 10);
};
