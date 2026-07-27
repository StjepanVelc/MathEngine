# MathEngine : Top-level CMake project file.
#
# Arhitektura: C++ matematička jezgra (core) -> WebAssembly (kasnije, Emscripten) -> web-aplikacija (web/)
# Baza podataka: za sad lokalno u browseru (SQLite/WASM, npr. sql.js), kasnije po potrebi server.
cmake_minimum_required (VERSION 3.14)

project ("MathEngine" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Izvorne datoteke su UTF-8 (logički simboli ∧ ∨ → ↔ u stringovima).
if (MSVC)
  add_compile_options(/utf-8)
endif()

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Matematička jezgra — builda se uvijek (native i Emscripten).
add_subdirectory(core)

if (EMSCRIPTEN)
  # WASM build: samo core + bindings (definirano u core/CMakeLists.txt).
else()
  # Native build: CLI aplikacija za razvoj/debug + testovi.
  add_subdirectory(app)

  enable_testing()
  add_subdirectory(tests)
endif()
