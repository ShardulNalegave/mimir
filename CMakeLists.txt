cmake_minimum_required(VERSION 3.15)
project(mimir VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(vendor/ccask)

find_package(spdlog REQUIRED)
find_package(Threads REQUIRED)

set(MIMIR_SOURCES
    "src/main.c"
    "src/server.c"
    "src/log.cc"
    "src/status.c")

add_executable(mimir ${MIMIR_SOURCES})

target_link_libraries(mimir
    PRIVATE
        ccask
        Threads::Threads
)

target_include_directories(mimir
    PRIVATE
        src/include
        ${CCASK_PUBLIC_INCLUDES}
        vendor/spdlog/include
)

if(MIMIR_ENABLE_LOGGING)
    target_compile_definitions(mimir PRIVATE MIMIR_ENABLE_LOGGING=1)
    target_link_libraries(mimir PRIVATE spdlog::spdlog)
endif()
