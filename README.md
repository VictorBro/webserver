
*This project has been created as part of the 42 curriculum by vbronov.*

# webserv

## Description

A non-blocking HTTP/1.1 server written in C++98, built from scratch without any external libraries.
The server supports GET, POST, and DELETE methods, file uploads, directory listing, CGI execution (Python, Perl), multiple virtual hosts, and configurable routing via a Nginx-inspired configuration file.
All I/O — client sockets, CGI pipes, and listening sockets — is driven by a single `epoll` event loop.

## Instructions

**Requirements:** Linux, `c++` (C++98), `make`

```bash
# Build
make

# Run with a config file
./webserv conf/default.conf

# Run with default config (conf/default.conf)
./webserv

# Build with debug output
DEBUG=1 make re

# Check memory leaks
make re && valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./webserv conf/default.conf

# Clean
make fclean
```

## Configuration

The config file follows a server/location block structure inspired by Nginx.

### Quick start example

```nginx
client_timeout            75
client_header_buffer_size 2k
client_max_body_size      1m

server {
    listen      0.0.0.0:8080;
    server_name localhost;
    root        /path/to/www;
    index       index.html;
    autoindex   off;

    cgi_bin .py /usr/bin/python3;
    cgi_bin .pl /usr/bin/perl;

    location /upload {
        allowed_methods  POST DELETE;
        upload_directory /path/to/www/uploads;
    }

    location /files {
        allowed_methods GET DELETE;
        autoindex       on;
    }
}
```

### Global defaults

These values apply when a directive is not explicitly specified:

| Directive | Default | Notes |
|---|---|---|
| `listen` | `0.0.0.0:80` | All interfaces, port 80 |
| `server_name` | *(empty)* | No virtual host name |
| `root` | `cwd/www` → `/var/www/html` | Resolved at startup |
| `index` | `index.html` | Default file for directory requests |
| `client_timeout` | `75` | Seconds before idle client is dropped |
| `client_header_buffer_size` | `2k` | Exceeding this returns 413 |
| `client_max_body_size` | `1m` | Exceeding this returns 413 |
| `allowed_methods` | `GET` | Methods permitted if not overridden |
| `autoindex` | `off` | Directory listing disabled |

### Global directives

Must appear outside any server block:

| Directive | Usage | Description |
|---|---|---|
| `client_timeout` | `client_timeout 75;` | Idle timeout in seconds |
| `client_header_buffer_size` | `client_header_buffer_size 2k;` | Header buffer limit; size suffix `k`/`m` supported |
| `client_max_body_size` | `client_max_body_size 1m;` | Request body limit; size suffix `k`/`m` supported |

### Server block directives

| Directive | Usage | Notes |
|---|---|---|
| `listen` | `listen ip:port;` | Multiple allowed; binds to address/port pair |
| `server_name` | `server_name name1 name2;` | Multiple declarations allowed |
| `root` | `root /path;` | Document root |
| `index` | `index file1 file2;` | Default files for directory requests |
| `error_page` | `error_page 404 /404.html;` | Multiple codes per line allowed |
| `allowed_methods` | `allowed_methods GET POST;` | Overrides global default |
| `autoindex` | `autoindex on;` | Directory listing |
| `cgi_bin` | `cgi_bin .py /usr/bin/python3;` | Maps extension to CGI executable |
| `return` | `return 301 http://example.com;` | Redirect or text response; takes precedence over all location blocks |

#### `return` directive

URLs must start with `http://` or `https://`. Text responses must be in double quotes.

```nginx
return 301 http://example.com;      # permanent redirect
return 302 https://example.com;     # temporary redirect
return 200 "OK";                    # text response
return 403 "Access Denied";
```

Valid status codes for redirects: `301`, `302`, `303`, `307`, `308`.
Valid range for text responses: `100`–`599`.

### Location block directives

Location blocks are nested inside a server block and apply to a URI prefix. More specific prefixes (e.g. `/foo/bar/`) take precedence over less specific ones (e.g. `/foo/`).

| Directive | Description |
|---|---|
| `root` | Override document root for this prefix |
| `index` | Override default index file(s) |
| `allowed_methods` | Override permitted HTTP methods |
| `autoindex` | Override directory listing |
| `return` | Redirect or text response; server-level `return` takes precedence |
| `upload_directory` | Directory where uploaded files are stored (passed as env var to CGI) |

#### `upload_directory` notes

The location containing `upload_directory` must be a prefix of the CGI upload script path. For example, if the upload script is served under `/cgi-bin`:

```nginx
location /cgi-bin {
    upload_directory /path/to/www/uploads/;
}
```

### Full configuration example

```nginx
# Global settings
client_timeout            75;
client_header_buffer_size 2k;
client_max_body_size      1m;

server {
    listen 80;
    listen 0.0.0.0:81;
    listen localhost:90;

    server_name example.org www.example.org;

    root    /var/www/html;
    index   index.html;
    autoindex on;
    allowed_methods GET POST DELETE;

    cgi_bin .pl  /usr/bin/perl;
    cgi_bin .py  /usr/bin/python3;

    error_page 404             /404.html;
    error_page 500 502 503 504 /50x.html;

    return 301 http://example.com/default;

    location / {
        index index.html;
    }

    location /foo/ {
        allowed_methods GET;
    }

    location /foo/bar/ {
        autoindex off;
    }

    location /special {
        return 302 http://example.com/special;
    }

    location /upload {
        root             /var;
        allowed_methods  POST;
        upload_directory /var/www/uploads;
    }
}

server {
    listen      localhost;
    server_name static.site;
    index       index.html home.html;
    error_page  404 /404.html;

    location / {
        autoindex off;
    }

    location /old-path {
        return 301 http://example.com/new-path;
    }
}
```

## Why epoll

The subject requires a single `poll()`-equivalent call to drive all I/O.
Four portable options exist; here is a brief comparison based on *The Linux Programming Interface* (Kerrisk, 2010), Chapter 63 — *Alternative I/O Models*:

| Mechanism | Portable | Max fds | Complexity | Notes |
|---|---|---|---|---|
| `select()` | POSIX | `FD_SETSIZE` (~1024) | O(n) | fd sets rebuilt on every call; hard fd ceiling |
| `poll()` | POSIX | unlimited | O(n) | No fd limit, but still scans the entire interest list every call |
| `epoll` | Linux only | unlimited | O(1) | Kernel maintains the interest list; only ready fds are returned |
| `kqueue` | BSD/macOS only | unlimited | O(1) | Equivalent to epoll on BSD systems |

`select` and `poll` both require the kernel to re-examine every watched descriptor on each call — as the number of connections grows the cost grows linearly. `epoll` avoids this by maintaining the interest list inside the kernel: the application only pays for descriptors that are actually ready (Kerrisk §63.4). For a server that can hold hundreds of concurrent connections across multiple listening sockets and CGI pipes at once, `epoll` is the correct choice on Linux.

`kqueue` would be the equivalent choice on macOS/BSD but is out of scope here since the server targets Linux only.

## Project Structure

```
src/
  Main.cpp
  server/     HttpServer  VirtualHost  Connection  ServerKey  Route  RouteTrie
  http/       HttpRequest  HttpResponse
  cgi/        CGI
  utils/      StringUtils  FileUtils  ProcUtils  Consts  Globals
conf/         default.conf  example.conf
www/          index.html  pages/  cgi-bin/  error/  style/
Makefile
```

## Resources

- [RFC 9110 — HTTP Semantics](https://www.rfc-editor.org/rfc/rfc9110)
- [RFC 9112 — HTTP/1.1](https://www.rfc-editor.org/rfc/rfc9112)
- Kerrisk, M. (2010). *The Linux Programming Interface*. No Starch Press. — Chapter 63: Alternative I/O Models
- [Nginx documentation](https://nginx.org/en/docs/) — reference for configuration file design
- [CGI/1.1 specification — RFC 3875](https://www.rfc-editor.org/rfc/rfc3875)

**AI usage:** GitHub Copilot was used to accelerate repetitive tasks such as writing boilerplate getters/setters, generating CGI environment variable lists, and drafting initial versions of error-handling branches. All generated code was reviewed, tested, and understood before being committed.

