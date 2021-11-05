project(wasm-engines)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-DV8_COMPRESS_POINTERS")

include_directories(v8/include)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

file(GLOB V8_LIB_MONOLIT "v8/x64.release/obj/libv8_monolith.a")

add_executable(wasm-engine main.cpp v8-instance-db.cpp)

target_link_libraries(wasm-engine Threads::Threads ${V8_LIB_MONOLIT}  ${CMAKE_DL_LIBS})

set(CMAKE_BUILD_TYPE "Release")
