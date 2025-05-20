###COMPILER###
CC = c++

###FLAGS###
CFLAGS = -Wextra -Wall -Werror -std=c++98 -I$(INCDIR) # -D SUBMISSION_BUILD=1
CPPCHECKFLAGS = -I$(INCDIR) --std=c++03
DEBUG_FLAGS = -g

###PROGRAM###
NAME = webserv

###DIRECTORIES###
SRCDIR = src
INCDIR = include
BINDIR = bin
OBJDIR = $(BINDIR)/obj
DEPDIR = $(BINDIR)/dep

###COLORS###
GREEN = \033[0;32m
YELLOW = \033[0;33m
RED = \033[0;31m
NC = \033[0m

#############################################################################################

SOURCES = $(shell find $(SRCDIR) -name '*.cpp')
HEADERS = $(shell find $(INCDIR) -name '*.hpp')
OBJECTS = $(SOURCES:%.cpp=$(OBJDIR)/%.o)
DEPS = $(SOURCES:%.cpp=$(DEPDIR)/%.d)

MAKEFLAGS += --jobs

.PHONY: all
all: $(NAME)

$(NAME): $(OBJECTS)
	@echo "$(YELLOW)Linking objects to create $(NAME)...$(NC)"
	$(CC) $(CFLAGS) -o $(NAME) $(OBJECTS)
	@echo "$(GREEN)Build successful!$(NC)"

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(DEPDIR)/$*)
	@echo "$(YELLOW)Compiling $<...$(NC)"
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@ -MF $(DEPDIR)/$*.d

.PHONY: run
run: all
	@echo "$(GREEN)Running $(NAME)...$(NC)"
	./$(NAME)

.PHONY: leak
leak: CFLAGS += $(DEBUG_FLAGS)
leak: all
	@echo "$(GREEN)Running $(NAME) with valgrind...$(NC)"
	valgrind --leak-check=full --show-leak-kinds=all --trace-children=yes --track-fds=all --track-origins=yes --error-exitcode=1 ./$(NAME)

.PHONY: debug
debug: CFLAGS += $(DEBUG_FLAGS)
debug: all
	@echo "$(GREEN)Launching debugger for $(NAME)...$(NC)"
	gdb ./$(NAME)

.PHONY: cppcheck
cppcheck:
	@echo "Running cppcheck..."
	cppcheck $(CPPCHECKFLAGS) --enable=all --error-exitcode=1 --suppress=missingIncludeSystem --suppress=unusedFunction --inline-suppr $(SRCDIR)/ $(INCDIR)/

.PHONY: strict
strict: all cppcheck
	@echo "$(GREEN)Strict build completed.$(NC)"

.PHONY: clean
clean:
	@echo "$(RED)Cleaning up...$(NC)"
	rm -rf $(BINDIR)
	rm -rf $(TESTBUILDDIR)

.PHONY: fclean
fclean: clean
	@echo "$(RED)Removing $(NAME)...$(NC)"
	rm -f $(NAME)

.PHONY: re
re: fclean
	$(MAKE) all

-include $(DEPS)