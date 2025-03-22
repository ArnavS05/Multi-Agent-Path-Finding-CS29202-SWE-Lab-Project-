cmake_minimum_required(VERSION 3.10)

# Project Name and Version
project(MultiAgentPathFinding VERSION 1.0)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")


# Include Directories
include_directories(include)


# All GUI related stuff:
# Find SFML package
set(SFML_DIR "/usr/lib/x86_64-linux-gnu/libsfml-graphics.so")
list(APPEND CMAKE_MODULE_PATH /usr/lib/x86_64-linux-gnu)
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
# GUI related stuff ends here



# Define Executables
add_executable(MAEPFProject main.cpp src/CBS.cpp src/AStar.cpp)
add_executable(MAEPFTest tests/TestCBS.cpp src/CBS.cpp src/AStar.cpp)


# Enable Testing
enable_testing()
add_test(NAME CBSTest COMMAND MAEPFTest)

# Link Libraries Correctly
target_link_libraries(MAEPFProject PRIVATE pthread dl util sfml-system sfml-window sfml-graphics)
# In the above line, the last 3 things are related to GUI



add_custom_target(coverage
    COMMAND ${LCOV_PATH} --capture --directory . --output-file coverage.info
    COMMAND ${LCOV_PATH} --remove coverage.info '/usr/*' '*/tests/*' '*/cmake_tests/include/*' --output-file coverage_filtered.info
    COMMAND ${GENHTML_PATH} coverage_filtered.info --output-directory coverage_report
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Creating coverage"
)

add_custom_target(coverage_clean
    COMMAND rm -rf ${CMAKE_BINARY_DIR}/coverage_report
    COMMAND rm -f ${CMAKE_BINARY_DIR}/coverage.info ${CMAKE_BINARY_DIR}/coverage_filtered.info
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Deleted the coverage files"
)
