# LocusFX

A standalone, open-source, high-performance web graphing library designed to rival desktop engines like Mathematica and standard web tools like Desmos.

## Features

- High-performance rendering using WebAssembly
- Mathematical expression parsing and evaluation
- Interactive graph manipulation
- Cross-platform browser support
- Lightweight with minimal dependencies

## Requirements

- CMake 3.20+
- C++20 compatible compiler (GCC 11+, Clang 13+, MSVC 2022+)
- Emscripten SDK (for WebAssembly builds)

## Building

### Native Build (for testing)

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### WebAssembly Build

```bash
mkdir build-wasm && cd build-wasm
emcmake cmake ..
cmake --build .
```

## Usage

```html
<script src="locus-fx.js"></script>
<script>
  const graph = new LocusFX.Graph('#container');
  graph.plot('sin(x)');
</script>
```

## Documentation

Documentation is available in the [docs](docs/) directory.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines on how to contribute.

## Security

See [SECURITY.md](SECURITY.md) for security policies and reporting vulnerabilities.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Inspired by Desmos and Mathematica
- Built with modern C++ and WebAssembly
