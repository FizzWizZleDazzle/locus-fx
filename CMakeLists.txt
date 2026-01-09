cmake_minimum_required(VERSION 3.20)

# Allow older cmake_minimum_required in dependencies (Eigen, SymEngine)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

# Suppress removed FindCUDA and FindBoost module warnings from Eigen
if(POLICY CMP0146)
    cmake_policy(SET CMP0146 OLD)
endif()
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 OLD)
endif()

project(LocusFX
    VERSION 0.1.0
    DESCRIPTION "High-performance web graphing library with implicit shader rendering"
    HOMEPAGE_URL "https://github.com/FizzWizZleDazzle/locus-fx"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(LOCUSFX_BUILD_TESTS "Build tests" ON)
option(LOCUSFX_BUILD_EXAMPLES "Build examples" ON)
option(LOCUSFX_BUILD_WASM "Build WebAssembly target" OFF)
option(LOCUSFX_CANVAS2D_FALLBACK "Enable Canvas2D fallback for non-WebGPU devices" ON)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif(MSVC)
    add_compile_options(/W4)
endif()

# =============================================================================
# Dependencies
# =============================================================================

include(FetchContent)

# Disable testing/docs for all dependencies
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
set(WITH_SYMENGINE_THREAD_SAFE ON CACHE BOOL "" FORCE)

# Eigen - Matrix math and regression (Least Squares)
FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0
    GIT_SHALLOW TRUE
    SYSTEM
    EXCLUDE_FROM_ALL
)

# SymEngine - Computer Algebra System (symbolic derivatives, expression parsing)
FetchContent_Declare(
    symengine
    GIT_REPOSITORY https://github.com/symengine/symengine.git
    GIT_TAG v0.12.0
    GIT_SHALLOW TRUE
    SYSTEM
    EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(eigen symengine)

# =============================================================================
# Source files
# =============================================================================

set(LOCUSFX_SOURCES
    src/locusfx.cpp
    src/codegen.cpp
)

set(LOCUSFX_HEADERS
    src/locusfx.h
    src/codegen.h
)

# =============================================================================
# Target configuration
# =============================================================================

if(EMSCRIPTEN OR LOCUSFX_BUILD_WASM)
    set(LOCUSFX_BUILD_WASM ON)

    add_executable(locus-fx ${LOCUSFX_SOURCES} ${LOCUSFX_HEADERS})

    # Emscripten WebGPU flags
    target_compile_options(locus-fx PRIVATE
        -sUSE_WEBGPU=1
    )

    # Build Emscripten link flags
    set(EMSCRIPTEN_LINK_FLAGS
        "-sWASM=1"
        "-sEXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
        "-sMODULARIZE=1"
        "-sEXPORT_NAME='LocusFX'"
        "-sUSE_WEBGPU=1"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sMAXIMUM_MEMORY=4GB"
        "-sSTACK_SIZE=1MB"
    )

    if(LOCUSFX_CANVAS2D_FALLBACK)
        target_compile_definitions(locus-fx PRIVATE LOCUSFX_CANVAS2D_FALLBACK=1)
    endif()

    list(JOIN EMSCRIPTEN_LINK_FLAGS " " EMSCRIPTEN_LINK_FLAGS_STR)

    set_target_properties(locus-fx PROPERTIES
        SUFFIX ".js"
        LINK_FLAGS "${EMSCRIPTEN_LINK_FLAGS_STR}"
    )
else()
    # Native build - static library (for testing/development)
    add_library(locus-fx STATIC ${LOCUSFX_SOURCES} ${LOCUSFX_HEADERS})

    # wgpu-native for native WebGPU support (optional, for testing)
    find_package(wgpu CONFIG QUIET)
    if(wgpu_FOUND)
        target_link_libraries(locus-fx PRIVATE wgpu::wgpu)
        target_compile_definitions(locus-fx PRIVATE LOCUSFX_HAS_NATIVE_WEBGPU=1)
    endif()
endif()

# Link dependencies
target_link_libraries(locus-fx
    PUBLIC
        Eigen3::Eigen
    PRIVATE
        symengine
)

target_include_directories(locus-fx
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${symengine_SOURCE_DIR}
        ${symengine_BINARY_DIR}
        ${symengine_BINARY_DIR}/symengine/utilities/teuchos
)

# Compile definitions
target_compile_definitions(locus-fx PRIVATE
    LOCUSFX_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    LOCUSFX_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    LOCUSFX_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

# =============================================================================
# Tests
# =============================================================================

if(LOCUSFX_BUILD_TESTS AND NOT LOCUSFX_BUILD_WASM)
    enable_testing()
    # add_subdirectory(tests)
endif()

# =============================================================================
# Examples
# =============================================================================

if(LOCUSFX_BUILD_EXAMPLES AND NOT LOCUSFX_BUILD_WASM)
    # add_subdirectory(examples)
endif()

# =============================================================================
# Installation
# =============================================================================

include(GNUInstallDirs)

install(TARGETS locus-fx
    EXPORT LocusFXTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES ${LOCUSFX_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/locusfx
)

install(EXPORT LocusFXTargets
    FILE LocusFXTargets.cmake
    NAMESPACE LocusFX::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LocusFX
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/LocusFXConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/LocusFXConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LocusFX
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/LocusFXConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/LocusFXConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/LocusFXConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LocusFX
)
