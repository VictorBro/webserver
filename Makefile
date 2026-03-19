# --- program names ---
NAME       := webserv
CLIENT     := client

# --- directories and sources ---
OBJDIR     := objs
SRCDIR     := src

SERVER_SRC := $(SRCDIR)/Main.cpp \
              $(SRCDIR)/server/HttpServer.cpp \
              $(SRCDIR)/server/VirtualHost.cpp \
              $(SRCDIR)/server/Connection.cpp \
              $(SRCDIR)/server/ServerKey.cpp \
              $(SRCDIR)/server/Route.cpp \
              $(SRCDIR)/server/RouteTrie.cpp \
              $(SRCDIR)/http/HttpRequest.cpp \
              $(SRCDIR)/http/HttpResponse.cpp \
              $(SRCDIR)/cgi/CGI.cpp \
              $(SRCDIR)/utils/StringUtils.cpp \
              $(SRCDIR)/utils/FileUtils.cpp \
              $(SRCDIR)/utils/ProcUtils.cpp \
              $(SRCDIR)/utils/Consts.cpp
CLIENT_SRC := $(SRCDIR)/client.cpp

SERVER_OBJ := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SERVER_SRC))
CLIENT_OBJ := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(CLIENT_SRC))

# --- compiler flags ---
CXX        := c++
CXXFLAGS   := -Wall -Wextra -Werror -std=c++98 -I $(SRCDIR)
DEBUG      ?= 0
SANITIZE   ?= 0

ifeq ($(DEBUG),1)
  CXXFLAGS += -DDEBUG=1 -g
endif
ifeq ($(SANITIZE),1)
  CXXFLAGS += -fsanitize=address
endif

# --- targets ---
.PHONY: all
all: $(NAME)
	@mkdir -p www/uploads

$(NAME): $(SERVER_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(CLIENT): $(CLIENT_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- housekeeping ---
.PHONY: clean fclean re
clean:
	rm -rf $(OBJDIR)
	rm -f www/uploads/*

fclean: clean
	rm -f $(NAME) $(CLIENT)

re: fclean all

# --- leak check ---
.PHONY: leak
leak: $(NAME)
	valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./$(NAME)
