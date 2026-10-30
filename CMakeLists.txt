cmake_minimum_required(VERSION 3.24)
project(marine_algorithms VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(MARINE_BUILD_TESTS "Build C++ tests" ${PROJECT_IS_TOP_LEVEL})
option(MARINE_SANITIZE "Build with AddressSanitizer + UBSan" OFF)

if(MARINE_SANITIZE)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

# C++ 코어
add_library(marine
    src/geodesy/haversine.cpp
    src/geodesy/vincenty.cpp
)
add_library(marine::marine ALIAS marine)
target_include_directories(marine PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_options(marine PRIVATE -Wall -Wextra -Wpedantic -Werror -ffp-contract=off)

# C 다리층
add_library(marine_c
    bindings/c/marine_c.cpp
)
add_library(marine::c ALIAS marine_c)
target_include_directories(marine_c PUBLIC ${PROJECT_SOURCE_DIR}/bindings/c/include)
target_link_libraries(marine_c PRIVATE marine)
target_compile_options(marine_c PRIVATE -Wall -Wextra -Wpedantic -Werror)

if(MARINE_BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.7.1
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(Catch2)

    enable_testing()
    add_executable(marine_tests
        tests/cpp/haversine_test.cpp
        tests/cpp/vincenty_test.cpp
        tests/cpp/c_api_test.cpp
    )
    target_link_libraries(marine_tests PRIVATE marine marine_c Catch2::Catch2WithMain)
    target_compile_definitions(marine_tests PRIVATE
        MARINE_TEST_DATA_DIR="${PROJECT_SOURCE_DIR}/tests/data")
    include(Catch)
    catch_discover_tests(marine_tests)
endif()
