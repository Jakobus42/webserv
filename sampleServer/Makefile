###COMPILER###
CC = c++

###FLAGS###
CFLAGS = -Wextra -Wall -Werror
DEBUG_FLAGS = -g

###PROGRAM###
NAME = sampleServer

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

format:
	@echo "Formatting code with clang-format..."
	clang-format -i $(SOURCES) $(HEADERS)

clean:
	@echo "$(RED)Cleaning up...$(NC)"
	rm -rf Testing
	rm -rf $(BINDIR)
	rm -rf $(TESTBUILDDIR)

fclean: clean
	@echo "$(RED)Removing $(NAME)...$(NC)"
	rm -f $(NAME)

re: fclean all

.PHONY: all re run format clean fclean