cmake_minimum_required(VERSION 3.30)
project(TapeSort)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

add_library(tapesort_lib
        src/tape/interface.h
        src/tape/file_tape.cpp
        src/tape/file_tape.h
        src/config/config.cpp
        src/config/config.h
        src/sorter/tape_sorter.cpp
        src/sorter/tape_sorter.h
        src/tape/tape_factory.cpp
        src/tape/tape_factory.h)

target_include_directories(tapesort_lib PUBLIC src)
target_link_libraries(tapesort_lib PUBLIC nlohmann_json::nlohmann_json)

add_executable(TapeSort src/main.cpp)
target_link_libraries(TapeSort PRIVATE tapesort_lib)

enable_testing()

add_executable(TapeSortTests
        tests/file_tape_test.cpp
        tests/tape_sorter_test.cpp
        tests/mock_tape.h
        tests/mock_tape.cpp
)
target_link_libraries(TapeSortTests PRIVATE tapesort_lib gtest_main)
add_test(NAME TapeSortTests COMMAND TapeSortTests)