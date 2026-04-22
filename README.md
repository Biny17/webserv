# 🌐 WebServ

A lightweight HTTP/1.1 web server written in C++98, built from scratch with no external dependencies.

> 42 School project — reimplementing core web server behavior from the ground up.

---

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Architecture](#architecture)
- [Getting Started](#getting-started)
- [Configuration](#configuration)
- [CGI Support](#cgi-support)
- [Project Structure](#project-structure)

---

## Overview

**WebServ** is a fully functional HTTP/1.1 server implemented in C++. It handles real browser requests, serves static content, and executes CGI scripts — all driven by a custom configuration file inspired by NGINX syntax.

Internally, it uses **`epoll`** (Linux) for I/O multiplexing, allowing the server to monitor multiple client connections simultaneously within a single thread — no blocking, no busy-waiting. Each event (new connection, incoming data, response ready) is dispatched through a non-blocking event loop, keeping the server responsive under concurrent load.

CGI processes are spawned via **`fork` + `execve`**, with communication handled through **pipes** (`stdin`/`stdout`). A timeout mechanism ensures hanging scripts don't stall the server indefinitely.

---

## Features

- ✅ **HTTP/1.1** — GET, POST, DELETE methods
- 📁 **Static file serving** — HTML, CSS, JS, images, and more
- 🛣️ **Static routes** — define URL-to-directory mappings via config
- ⚙️ **CGI support** — execute scripts (Python, PHP, etc.) on the server side
- 📄 **Config file** — flexible server configuration (ports, hosts, routes, error pages...)
- 🚫 **Custom error pages** — 404, 403, 500 and more
- 🔁 **Redirections** — HTTP 301/302 support
- 📦 **No external dependencies** — pure C++98 stdlib only

---

## Architecture

### Non-blocking I/O with epoll

WebServ uses **`epoll`** (Linux) to handle multiple concurrent connections within a single thread. All sockets — server and clients — are set to non-blocking mode. The event loop monitors file descriptors and reacts only when data is actually ready, avoiding busy-waiting entirely.

```
epoll_wait() ──► event on fd
                    ├── server socket  →  accept() new client
                    ├── client socket  →  read request / send response
                    └── CGI pipe       →  read script output
```

This means the server never blocks on a single slow client, and can handle many simultaneous connections efficiently without threading.

### Request lifecycle

```
accept()  →  parse HTTP request  →  match location block
          →  serve file / redirect / exec CGI
          →  build HTTP response  →  send()  →  close or keep-alive
```

### CGI execution

CGI scripts are launched via **`fork()` + `execve()`**. The server communicates with the child process through a **pipe**: request body is written to `stdin`, and the script's `stdout` is read back as the response body. A timeout mechanism kills stalled CGI processes to prevent the server from hanging.

---

## Getting Started

### Requirements

- `clang++`
- `make`
- Unix-based OS (Linux / macOS)

### Build

```bash
git clone https://github.com/Biny17/webserv.git
cd webserv
make
```

### Run

```bash
./webserv [config_file]
```

If no config file is provided, the server uses a default configuration.

```bash
./webserv config/server.conf
```

Then open your browser at `http://localhost:8080`.

---

## Configuration

The config file follows an NGINX-inspired syntax. Here's a minimal example:

```nginx
server {
    listen       8080;
    server_name  localhost;

    root         ./www;
    index        index.html;

    error_page   404 /errors/404.html;

    location / {
        allow_methods  GET POST;
        autoindex      on;
    }

    location /upload {
        allow_methods  POST;
        root           ./www/uploads;
    }
}
```

### Supported directives

| Directive               | Description                           |
|-------------------------|---------------------------------------|
| `listen`                | Port to listen on                     |
| `server_name`           | Virtual host name                     |
| `root`                  | Root directory for file serving       |
| `index`                 | Default file to serve                 |
| `error_page`            | Custom error pages                    |
| `allow_methods`         | Allowed HTTP methods per location     |
| `autoindex`             | Enable/disable directory listing      |
| `return`                | HTTP redirect                         |
| `cgi_extension`         | Allowed CGI extensions                |
| `client_max_body_size`  | Max request body size                 |

---

## CGI Support

WebServ can execute CGI scripts based on file extension.

```nginx
location /cgi-bin {
    allow_methods  GET POST;
    cgi_extension  .py .sh .rb;
}
```

CGI scripts receive request data via environment variables and `stdin`, and return their response through `stdout` — following the CGI/1.1 standard.

---

## Project Structure

```
webserv/
├── cgi-bin/                # CGI scripts
├── config/                 # Configuration files
├── headers/                # Header files (.hpp)
├── srcs/                   # Source files (.cpp)
│   ├── cgi/                # CGI execution (fork, pipe, env)
│   ├── classes/            # Core classes (Server, Client, Request, Response...)
│   ├── conf/               # Config file parser
│   ├── handle_request/     # GET / DELETE response logic
│   ├── network/            # Epoll event loop, client handling
│   └── utils/              # Helpers (autoindex, encoding, path...)
├── www/                    # Default web root
├── Makefile
└── README.md
```

---

## Authors

| Name | GitHub |
|------|--------|
| Anicet Gruet | [@Anicet78](https://github.com/Anicet78) |
| Tom Pinton | [@Snak00s](https://github.com/Snak00s) |
| Tristan Biny | [@Biny17](https://github.com/Biny17) |

---
