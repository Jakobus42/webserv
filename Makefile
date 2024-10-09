###COMPILER###
CC = c++

###FLAGS###
CFLAGS = -Wextra -Wall -Werror -std=c++98
DEBUG_FLAGS = -g

###PROGRAM###
NAME = template

###DIRECTORIES###
SRCDIR = src
INCDIR = include
BINDIR = bin
DOCDIR = docs
OBJDIR = $(BINDIR)/obj
DEPDIR = $(BINDIR)/dep

#############################################################################################

GREEN = \033[0;32m
YELLOW = \033[0;33m
RED = \033[0;31m
NC = \033[0m

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

#############################################################################################

all: $(NAME)

run: all
	@echo "$(GREEN)Running $(NAME)...$(NC)"
	./$(NAME)

leak: all
	@echo "$(GREEN)Running $(NAME) with valgrind...$(NC)"
	valgrind --leak-check=full --error-exitcode=1 ./$(NAME)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: all
	@echo "$(GREEN)Launching debugger for $(NAME)...$(NC)"
	gdb ./$(NAME)

cppcheck:
	@echo "Running cppcheck..."
	cppcheck --enable=all --error-exitcode=1 --suppress=missingIncludeSystem --suppress=missingOverride --suppress=noExplicitConstructor --suppress=unusedFunction $(SRCDIR)/ $(INCDIR)/

strict: all cppcheck leak
	@echo "$(GREEN)Strict build completed.$(NC)"

docs: all
	@echo "Generating documentation with Doxygen..."
	doxygen Doxyfile

format:
	@echo "Formatting code with clang-format..."
	clang-format -i $(SOURCES) $(HEADERS)

clean:
	@echo "$(RED)Cleaning up...$(NC)"
	rm -rf $(BINDIR)

fclean: clean
	@echo "$(RED)Removing $(NAME)...$(NC)"
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re run debug cppcheck docs strict leak format