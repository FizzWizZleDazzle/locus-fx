# LocusFX

A standalone, open-source, high-performance web graphing library designed to rival desktop engines like Mathematica and standard web tools like Desmos. LocusFX decouples rendering logic from the main Locus App platform, enabling third-party developers to leverage the same technology.

## Core Technology

| Component | Technology | Purpose |
|-----------|------------|---------|
| Language | C++20 → WebAssembly | High performance via Emscripten |
| Math Kernel | SymEngine | CAS for symbolic derivatives & expression parsing |
| Linear Algebra | Eigen | Matrix math & regression (Least Squares) |
| Graphics | WebGPU (wgpu) | Compute-shader-driven rendering |
| Fallback | Canvas2D | Compatibility for non-WebGPU devices |

## Rendering Pipeline

LocusFX uses **Implicit Shader Rendering** to achieve infinite resolution and zoom:

1. **Input**: User provides a string (e.g., `sin(x) * y = 1`)
2. **Transpilation**: C++ kernel parses the string, calculates symbolic derivatives (∇f), and generates WGSL shader code on the fly
3. **Space Skipping**: Compute shader runs Interval Arithmetic on a Quadtree grid to identify which 32×32 pixel tiles contain the curve
4. **Drawing**: Fragment shader evaluates the distance field per pixel using the gradient (|f|/|∇f|), ensuring constant line width at any zoom level (10¹⁵×)

## Key Features

- **Infinite Zoom**: No jagged edges or blockiness at any magnification
- **Implicit Equations**: Native support for complex relations like x² + y² = r² without solving for y
- **Integrals**: GPU Parallel Prefix Sum (Scan) for instant area accumulation
- **Parametric & Piecewise**: Shader branching and interval merging
- **Regression**: Instant Least Squares via Eigen, renders resulting function

## Requirements

- CMake 3.20+
- C++20 compatible compiler (GCC 11+, Clang 13+, MSVC 2022+)
- Emscripten SDK 3.1.50+ (for WebAssembly builds)

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
emcmake cmake .. -DLOCUSFX_BUILD_WASM=ON
cmake --build .
```

### Build Options

| Option | Default | Description |
|--------|---------|-------------|
| `LOCUSFX_BUILD_TESTS` | ON | Build test suite |
| `LOCUSFX_BUILD_EXAMPLES` | ON | Build example applications |
| `LOCUSFX_BUILD_WASM` | OFF | Build WebAssembly target |
| `LOCUSFX_CANVAS2D_FALLBACK` | ON | Enable Canvas2D fallback |

## Usage

```html
<script src="locus-fx.js"></script>
<script>
  LocusFX().then(lib => {
    const graph = new lib.Graph('#container');

    // Explicit function
    graph.plot('sin(x)');

    // Implicit equation
    graph.plot('x^2 + y^2 = 4');

    // Parametric
    graph.plotParametric('cos(t)', 'sin(t)', { t: [0, 2 * Math.PI] });
  });
</script>
```

## Architecture

```
src/
├── core/           # Expression parsing, AST
├── cas/            # SymEngine integration, symbolic derivatives
├── codegen/        # WGSL shader generation
├── render/         # WebGPU pipeline, quadtree space-skipping
├── math/           # Eigen integration, regression
└── fallback/       # Canvas2D renderer
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

- [SymEngine](https://github.com/symengine/symengine) - Symbolic computation
- [Eigen](https://eigen.tuxfamily.org/) - Linear algebra
- [wgpu](https://wgpu.rs/) - WebGPU implementation
