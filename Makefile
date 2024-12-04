
###COMPILER###
CC = c++

###FLAGS###
CFLAGS = -Wextra -Wall -Werror -std=c++98 -I$(INCDIR)
CPPCHECKFLAGS = -I$(INCDIR)
DEBUG_FLAGS = -g

###PROGRAM###
NAME = webserv

###DIRECTORIES###
SRCDIR = src
INCDIR = include
BINDIR = bin
TESTBUILDDIR = build
OBJDIR = $(BINDIR)/obj
DEPDIR = $(BINDIR)/dep

#############################################################################################

GREEN = \033[0;32m
YELLOW = \033[0;33m
RED = \033[0;31m
NC = \033[0m

CLANG_STYLE = --style=file:./.clang-format-default
CLANG_FORMAT = clang-format

SOURCES = $(shell find $(SRCDIR) -name '*.cpp')
HEADERS = $(shell find $(INCDIR) -name '*.hpp')
OBJECTS = $(SOURCES:%.cpp=$(OBJDIR)/%.o)
DEPS = $(SOURCES:%.cpp=$(DEPDIR)/%.d)

$(NAME): $(OBJECTS)
	@echo "$(YELLOW)Linking objects to create $(NAME)...$(NC)"
	$(CC) $(CFLAGS) -o $(NAME) $(OBJECTS)
	@echo "$(GREEN)Build successful!$(NC)"

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(DEPDIR)/$*)
	@echo "$(YELLOW)Compiling $<...$(NC)"
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@ -MF $(DEPDIR)/$*.d

-include $(DEPS)

NUM_PROCS = $(shell nproc)
MAKEFLAGS = -j$(NUM_PROCS)

#############################################################################################

all: $(NAME)

run: all
	@echo "$(GREEN)Running $(NAME)...$(NC)"
	./$(NAME)

leak: all
	@echo "$(GREEN)Running $(NAME) with valgrind...$(NC)"
	valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes --track-origins=yes --error-exitcode=1 ./$(NAME)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: all
	@echo "$(GREEN)Launching debugger for $(NAME)...$(NC)"
	gdb ./$(NAME)

cppcheck:
	@echo "Running cppcheck..."
	cppcheck $(CPPCHECKFLAGS) --enable=all --error-exitcode=1 --suppress=missingIncludeSystem --suppress=missingOverride --suppress=noExplicitConstructor --suppress=unusedFunction $(SRCDIR)/ $(INCDIR)/

strict: all cppcheck leak
	@echo "$(GREEN)Strict build completed.$(NC)"

build_tests:
	@echo "$(GREEN)Building tests...$(NC)"
	mkdir -p $(TESTBUILDDIR)
	cd $(TESTBUILDDIR) && cmake .. && cmake --build .

test: build_tests
	@echo "$(GREEN)Testing code...$(NC)"
	cd build && ctest


format:
	@echo "Formatting code with clang-format..."
	$(CLANG_FORMAT) $(CLANG_STYLE) -i $(SOURCES) $(HEADERS)

clean:
	@echo "$(RED)Cleaning up...$(NC)"
	rm -rf Testing
	rm -rf $(BINDIR)
	rm -rf $(TESTBUILDDIR)

fclean: clean
	@echo "$(RED)Removing $(NAME)...$(NC)"
	rm -f $(NAME)

re: fclean
	$(MAKE) all

.PHONY: all clean fclean re run debug cppcheck strict leak format
