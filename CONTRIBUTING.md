# Contributing to GStreamer Pipeline Studio

Thank you for your interest in contributing to GStreamer Pipeline Studio! We
welcome contributions from developers of all skill levels.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [How to Contribute](#how-to-contribute)
- [Development Setup](#development-setup)
- [Coding Standards](#coding-standards)
- [Submitting Changes](#submitting-changes)
- [Reporting Issues](#reporting-issues)
- [Development Workflow](#development-workflow)
- [Testing Guidelines](#testing-guidelines)

## Code of Conduct

This project adheres to a code of conduct that we expect all contributors to follow:

- **Be respectful**: Treat everyone with respect and kindness
- **Be inclusive**: Welcome newcomers and help them learn
- **Be constructive**: Provide helpful feedback and suggestions
- **Be patient**: Remember that everyone has different experience levels

## How to Contribute

### Types of Contributions

We welcome several types of contributions:

- 🐛 **Bug reports**: Help us identify and fix issues
- ✨ **Feature requests**: Suggest new functionality
- 🔧 **Code contributions**: Implement features or fix bugs
- 📝 **Documentation**: Improve README, wiki, or code comments
- 🎨 **UI/UX improvements**: Enhance the user interface
- 🧪 **Testing**: Add test cases or improve test coverage
- 🌐 **Translations**: Help localize the application

### Getting Started

1. **Fork the repository** on GitHub
2. **Clone your fork** locally
3. **Create a feature branch** from `develop`
4. **Make your changes** following our coding standards
5. **Test your changes** thoroughly
6. **Submit a pull request**

## Development Setup

### Prerequisites

```bash
# Install dependencies (Arch Linux / Manjaro)
sudo pacman -S qt6-base qt6-declarative qt6-quickcontrols2 gstreamer cmake git

# Ubuntu / Debian
sudo apt install qt6-base-dev qt6-declarative-dev qt6-quickcontrols2-dev \
                 libgstreamer1.0-dev cmake git

# Fedora
sudo dnf install qt6-qtbase-devel qt6-qtdeclarative-devel \
                 qt6-qtquickcontrols2-devel gstreamer1-devel cmake git
```

### Building from Source

```bash
# Clone your fork
git clone https://github.com/YOUR_USERNAME/gststudio.git
cd gststudio

# Create and switch to develop branch
git checkout -b develop origin/develop

# Build the project
mkdir build && cd build
cmake ..
make -j$(nproc)

# Run the application
./gst-pipeline-studio
```

### Development Environment

**Recommended IDEs:**

- Qt Creator (preferred for Qt/QML development)
- CLion (excellent CMake support)
- VS Code with Qt and C++ extensions

**Useful Tools:**

- `qmlformat` for QML code formatting
- `clang-format` for C++ code formatting
- `gst-inspect-1.0` for testing GStreamer elements

**Key Files to Understand:**

- `main.cpp` - Application initialization and QML registration
- `Main.qml` - Primary user interface
- `gstelementbrowser.h/.cpp` - Core browser functionality
- `gstinspectparser.h/.cpp` - GStreamer element parsing logic
- `gst*model.h/.cpp` - Data models for QML interface

## Coding Standards

### C++ Code Style

We follow Qt's coding conventions with some modifications:

```cpp
// Class names: PascalCase (following actual project structure)
class GstElementBrowser : public QObject
{
    Q_OBJECT

public:
    // Function names: camelCase
    explicit GstElementBrowser(QObject *parent = nullptr);
    void parseElements();

private:
    // Member variables: m_ prefix, camelCase
    QString m_elementName;
    QList<GstProperty> m_properties;

    // Private functions: camelCase
    void updateElementDetails();
};
```

**File Naming Convention (as used in project):**

- Header files: `gstclassname.h` (lowercase)
- Source files: `gstclassname.cpp` (lowercase)
- Class names: `GstClassName` (PascalCase)

### QML Style

```qml
// Main.qml structure (following project layout)
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    id: root

    // Properties: camelCase
    property string selectedElement: ""

    // Components follow project structure
    GstElementBrowser {
        id: elementBrowser
    }
}
```

### Formatting

- **Indentation**: 4 spaces (no tabs)
- **Line length**: Maximum 120 characters
- **Braces**: Opening brace on same line
- **Comments**: Use `//` for single-line, `/* */` for multi-line

### Pre-commit Formatting

```bash
# Install clang-format
sudo pacman -S clang  # Arch
sudo apt install clang-format  # Ubuntu

# Format C++ files in src/lib/
find src/lib/ -name "*.cpp" -o -name "*.h" | xargs clang-format -i

# Format main files
clang-format -i main.cpp

# Format QML files (if qmlformat is available)
qmlformat -i Main.qml
```

## Submitting Changes

### Branch Naming

Use descriptive branch names:

- `feature/visual-pipeline-editor`
- `bugfix/parser-memory-leak`
- `docs/api-documentation`
- `refactor/element-model-cleanup`

### Commit Messages

Follow the Conventional Commits specification:

```text
<type>[optional scope]: <description>

[optional body]

[optional footer(s)]
```

**Types:**

- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (no logic changes)
- `refactor`: Code refactoring
- `test`: Adding or modifying tests
- `chore`: Maintenance tasks

**Examples:**

```text
feat(parser): add support for GStreamer 1.24 elements

fix(ui): resolve Material theme issues on KDE systems

docs(readme): update installation instructions for Fedora

refactor(model): simplify property parsing logic
```

### Pull Request Process

1. **Update your branch** with the latest `develop`
2. **Run tests** and ensure they pass
3. **Update documentation** if needed
4. **Create a pull request** with:
   - Clear title and description
   - Reference to related issues
   - Screenshots for UI changes
   - Testing instructions

### Pull Request Template

```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Documentation update
- [ ] Refactoring

## Testing
- [ ] Tested on Linux
- [ ] Tested with different GStreamer versions
- [ ] Added/updated tests

## Screenshots (if applicable)
[Add screenshots for UI changes]

## Related Issues
Fixes #123
```

## Reporting Issues

### Bug Reports

Use our bug report template:

```markdown
**Describe the bug**
A clear description of what the bug is.

**To Reproduce**
Steps to reproduce the behavior:
1. Go to '...'
2. Click on '....'
3. See error

**Expected behavior**
What you expected to happen.

**Environment:**
- OS: [e.g., Manjaro KDE 6]
- Qt Version: [e.g., 6.5.0]
- GStreamer Version: [e.g., 1.22.1]
- Application Version: [e.g., v0.1.0]

**Additional context**
Add any other context about the problem.
```

### Feature Requests

```markdown
**Is your feature request related to a problem?**
A clear description of what the problem is.

**Describe the solution you'd like**
A clear description of what you want to happen.

**Describe alternatives you've considered**
Other solutions you've considered.

**Additional context**
Add any other context or screenshots.
```

## Development Workflow

### Project Structure

```
gststudio/
├── CMakeLists.txt       # Main CMake configuration
├── Generated/           # Generated files (build artifacts)
├── main.cpp            # Application entry point
├── Main.qml            # Main QML interface
└── src/
    ├── CMakeLists.txt  # Source CMake configuration
    └── lib/            # Core library implementation
        ├── CMakeLists.txt
        ├── gstelementbrowser.cpp/.h    # Main browser component
        ├── gstelement.h                # Element data structures
        ├── gstinspectparser.cpp/.h     # GStreamer inspect parser
        ├── gstpadmodel.cpp/.h          # Pad template model
        └── gstpropertymodel.cpp/.h     # Property model
```

### Git Workflow

We use Git Flow with these branches:

- `main`: Stable releases only
- `develop`: Integration branch for features
- `feature/*`: New features
- `bugfix/*`: Bug fixes
- `release/*`: Release preparation

## Testing Guidelines

### Running Tests

```bash
# Build with tests (when implemented)
cmake -DBUILD_TESTS=ON ..
make -j$(nproc)

# Run all tests (when available)
ctest

# Currently: Manual testing
./gststudio
```

### Writing Tests

When the test framework is added:

- Write unit tests for parser functions in `src/lib/`
- Test both success and failure cases for `gstinspectparser.cpp`
- Use descriptive test names
- Mock external dependencies (GStreamer calls)

Example for future testing:

```cpp
// Test for gstinspectparser.cpp
TEST_CASE("GstInspectParser parses element properties correctly") {
    GstInspectParser parser;
    QString testOutput = "..."; // Mock gst-inspect output

    GstElement element = parser.parseElementDetails(testOutput);

    REQUIRE(element.properties.size() == 3);
    REQUIRE(element.properties[0].name == "automatic-eos");
    REQUIRE(element.properties[0].type == "Boolean");
}
```

## Getting Help

- 💬 **GitHub Discussions**: General questions and ideas
- 🐛 **GitHub Issues**: Bug reports and feature requests
- 📧 **Email**: <maintainer@example.com> for private inquiries
- 🗨️ **Chat**: Join our development chat (link)

## Recognition

Contributors will be recognized in:

- `CONTRIBUTORS.md` file
- Release notes
- About dialog in the application

Thank you for contributing to GStreamer Pipeline Studio! 🎉
