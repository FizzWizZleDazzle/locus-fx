# Contributing to LocusFX

Thank you for your interest in contributing to LocusFX! This document provides guidelines and instructions for contributing.

## Code of Conduct

By participating in this project, you agree to abide by our [Code of Conduct](CODE_OF_CONDUCT.md).

## How to Contribute

### Reporting Bugs

1. Check existing issues to avoid duplicates
2. Use the bug report template
3. Include:
   - Clear description of the issue
   - Steps to reproduce
   - Expected vs actual behavior
   - Browser/environment information
   - Relevant code snippets or screenshots

### Suggesting Features

1. Check existing feature requests
2. Open an issue with the feature request template
3. Describe the use case and proposed solution

### Pull Requests

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/your-feature-name`
3. Make your changes
4. Write or update tests as needed
5. Ensure all tests pass
6. Submit a pull request

## Development Setup

### Prerequisites

- CMake 3.20+
- C++20 compatible compiler
- Emscripten SDK (for WebAssembly builds)
- Git

### Building

```bash
# Clone the repository
git clone https://github.com/FizzWizZleDazzle/locus-fx.git
cd locus-fx

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
cmake --build .
```

### Running Tests

```bash
cd build
ctest --output-on-failure
```

## Coding Standards

### C++ Style

- Use C++20 features where appropriate
- Follow the project's existing code style
- Use meaningful variable and function names
- Keep functions focused and concise
- Document public APIs with comments

### Formatting

- Use 4 spaces for indentation
- Opening braces on the same line
- Maximum line length: 100 characters

### Commit Messages

- Use present tense ("Add feature" not "Added feature")
- Use imperative mood ("Move cursor to..." not "Moves cursor to...")
- Keep the first line under 72 characters
- Reference issues and pull requests where relevant

## Review Process

1. All submissions require review
2. Maintainers may request changes
3. Once approved, maintainers will merge the PR

## License

By contributing, you agree that your contributions will be licensed under the MIT License.
