# GStreamer Pipeline Studio

A modern, visual tool for creating and managing GStreamer multimedia pipelines with
automatic code generation.

## Overview

GStreamer Pipeline Studio is a Qt6-based graphical application that provides an
intuitive interface for building complex GStreamer pipelines. By combining visual
node-based editing with dynamic element discovery, it bridges the gap between
GStreamer's powerful capabilities and user-friendly pipeline creation.

### Key Features

- **Visual Pipeline Editor** - Drag-and-drop interface for connecting GStreamer
  elements
- **Dynamic Element Discovery** - Automatically detects all installed GStreamer
  plugins using `gst-inspect`
- **Real-time Property Editing** - Configure element properties through
  dynamically generated GUI controls
- **Code Generation** - Export working pipelines as clean C++ or Python code
- **Element Browser** - Searchable catalog of available elements with detailed
  documentation
- **Compatibility Validation** - Visual feedback for valid/invalid element
  connections
- **Modern UI** - Material Design dark theme with professional styling

## Current Status

**Phase 1: Element Browser** ✅ *Completed*

- GStreamer element discovery and parsing
- Dynamic property and pad template display
- Search and filtering capabilities
- Qt6 + QML interface

**Phase 2: Visual Pipeline Editor** 🚧 *In Development*

- Node-based pipeline canvas
- Drag-and-drop element placement
- Visual connection system
- Real-time pipeline validation

**Phase 3: Code Generation** 📋 *Planned*

- C++ code export with proper error handling
- Python code generation (GObject and modern bindings)
- Pipeline string export
- Template and boilerplate code

## Technology Stack

- **Frontend**: Qt6 with QML and Material Design
- **Backend**: C++ with Qt6 Core/Quick
- **Parser**: Custom gst-inspect output parser
- **Styling**: Material Dark theme with KDE compatibility
- **Build System**: CMake

## Screenshots

![Element Browser](screenshots/element-browser.png)

*Element Browser showing available GStreamer plugins with detailed property
information*

## Installation

### Prerequisites

```bash
# Arch Linux / Manjaro
sudo pacman -S qt6-base qt6-declarative qt6-quickcontrols2 gstreamer

# Ubuntu / Debian
sudo apt install qt6-base-dev qt6-declarative-dev qt6-controls2-dev \
                 libgstreamer1.0-dev

# Fedora
sudo dnf install qt6-qtbase-devel qt6-qtdeclarative-devel \
                 qt6-qtquickcontrols2-devel gstreamer1-devel
```

### Building

```bash
git clone https://github.com/drook207/gststudio.git
cd gststudio
mkdir build && cd build
cmake ..
make -j$(nproc)
./gst-pipeline-studio
```

### Running

```bash
# Standard launch
./gst-pipeline-studio

# Force Material Dark theme (KDE systems)
QT_QPA_PLATFORMTHEME="" QT_QUICK_CONTROLS_STYLE=Material ./gst-pipeline-studio
```

## Usage

1. **Browse Elements**: Use the left panel to explore available GStreamer elements
2. **Search & Filter**: Find specific elements using the search functionality
3. **View Details**: Click any element to see its properties, pad templates, and
   capabilities
4. **Understand Properties**: See property types, default values, ranges, and
   access flags
5. **Explore Pads**: View source/sink pad capabilities and connection requirements

## Development Roadmap

### Near Term

- [ ] Node-based visual pipeline editor
- [ ] Element placement and connection system
- [ ] Real-time pipeline validation
- [ ] Basic code generation

### Medium Term

- [ ] Advanced property editing widgets
- [ ] Pipeline templates and presets
- [ ] Plugin system for custom elements
- [ ] Pipeline testing and debugging tools

### Long Term

- [ ] Remote pipeline monitoring
- [ ] Collaborative editing features
- [ ] Integration with GStreamer development tools
- [ ] Cross-platform deployment

## Architecture

```text
┌─────────────────┬─────────────────┐
│   QML Frontend  │  Element Browser │
├─────────────────┼─────────────────┤
│ GstElementBrowser (C++)            │
├─────────────────┼─────────────────┤
│ GstInspectParser│ Property Models │
├─────────────────┼─────────────────┤
│    GStreamer    │   gst-inspect   │
└─────────────────┴─────────────────┘
```

## Contributing

We welcome contributions! Please see our [Contributing Guidelines](CONTRIBUTING.md) for details.

### Development Setup

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/amazing-feature`
3. Make your changes and test thoroughly
4. Submit a pull request with a clear description

### Code Style

- C++: Follow Qt coding conventions
- QML: Use consistent indentation and Material Design patterns
- Commit messages: Use conventional commit format

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- GStreamer development team for the powerful multimedia framework
- Qt Project for the excellent cross-platform toolkit
- The open-source community for inspiration and feedback

## Support

- **Issues**: Report bugs and feature requests on [GitHub Issues](https://github.com/drook207/gststudio/issues)
- **Discussions**: Join conversations in [GitHub Discussions](https://github.com/drook207/gststudio/discussions)
- **Documentation**: Visit our [Wiki](https://github.com/drook207/gststudio/wiki)

---

> "Making GStreamer pipeline creation as visual and intuitive as the multimedia
> content it processes."
