# Getting Started

## Table of Contents
1. [Development Workflow](#development-workflow)
2. [Conventions](#getting-started)
3. [Building and Testing](#building-and-testing)
4. [Documentation](#documentation)
5. [Pull Requests](#pull-requests)
6. [Docker and VS Code Integration](#docker-and-vs-code-integration)
7. [Quick Command Reference](#quick-command-reference)

## Development Workflow

### Pre-Commit Hooks
- Automatic checks run on each commit:
  - Code formatting
  - Commit message validation
- To manually format code:
  ```
  make format
  ```
  This applies Google style formatting to your code.

### New Class Creation
Use the provided script to create new classes:

```
./createClass.sh <subfolder> <className>
```

Example:
```
./createClass.sh logger LoggerManager
```

This will:
- Create a `.hpp` file in `include/<subfolder>/`
- Create a `.cpp` file in `src/<subfolder>/`
- Generate a class with default constructor, destructor, copy constructor, copy assignment operator, and Doxygen documentation

## Conventions

### Naming Conventions
- Class Names: PascalCase (e.g., `UserManager`)
- Function Names: camelCase, start with a verb (e.g., `getUserById()`)
- Variable Names: camelCase (e.g., `userCount`)
- Constants: ALL_CAPS with underscores (e.g., `MAX_USERS`)
- Private Member Variables: Prefix with `m_` followed by camelCase (e.g., `m_userCount`)
- Enum Names: PascalCase for enum name, ALL_CAPS for values
- Namespace Names: lowercase (e.g., `namespace utils {}`)

### Branch Naming Convention
Create a new branch for your work using one of the following prefixes:
- `feat/`: For new features
- `bugfix/`: For bug fixes
- `hotfix/`: For critical bug fixes that need immediate attention
- `chore/`: For maintenance tasks
- `sandbox/`: For experimental work or testing

Example: `feat/add-user-authentication`

Avoid working directly on the `main` branch.

### Commit Message Convention
Use clear, descriptive commit messages following this format:
```
<type>: <description>
[optional body]
[optional footer]
```

Allowed types:
- `feat`: A new feature
- `fix`: A bug fix
- `chore`: Routine tasks, maintenance, or refactors
- `docs`: Documentation changes
- `test`: Adding or modifying tests
- `refactor`: Code changes that neither fix a bug nor add a feature
- `style`: Changes that do not affect the meaning of the code (white-space, formatting, etc.)
- `ci`: Changes to CI configuration files and scripts
- `build`: Changes that affect the build system or external dependencies

## Building and Testing

### Compilation
```
make all
```

### Running the Project
```
make run
```

### Static Analysis
```
make cppcheck
```

### Memory Checks
```
make leak
```

### Comprehensive Check
```
make strict
```
This runs both static analysis and memory checks.

## Pull Requests
1. Ensure all tests pass, code is properly formatted and documented
2. Open a Pull Request when your work is ready for review
3. Provide a clear description of the changes in your PR

## Docker and VS Code Integration

### Building Docker Image
```
docker build -t template .
```

### Running Container
```
docker run -it --rm -v ~/.ssh:/root/.ssh -v ~/.zshrc:/root/.zshrc template
```

### VS Code Setup
1. Press `Cmd + P` (or `Ctrl + P` on Windows/Linux)
2. Type `> Reopen in Container`
3. Select the appropriate container configuration

## Quick Command Reference

| Command | Description |
|---------|-------------|
| `make all` | Compile the project |
| `make run` | Run the project |
| `make format` | Format the code |
| `make cppcheck` | Run static analysis |
| `make leak` | Perform memory checks |
| `make strict` | Run full check (static analysis + memory checks) |

Happy coding!