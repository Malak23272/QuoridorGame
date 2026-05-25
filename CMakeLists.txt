cmake_minimum_required(VERSION 3.16)
project(QuoridorGame VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Concurrent)

set(SOURCES
    src/main.cpp
    src/Core/Board.cpp
    src/Core/GameEngine.cpp
    src/Core/Player.cpp
    src/AI/AIPlayer.cpp
    src/UI/GameWindow.cpp
    src/UI/GameBoardWidget.cpp
    src/UI/Input.cpp
)

set(HEADERS
    include/Core/Types.h
    include/Core/Board.h
    include/Core/GameEngine.h
    include/Core/Player.h
    include/AI/AIPlayer.h
    include/AI/Minmax.h
    include/UI/GameWindow.h
    include/UI/GameBoardWidget.h
    include/UI/Input.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
    include
    include/Core
    include/AI
    include/UI
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Concurrent)

# Test target (manual AUTOMOC off)
add_executable(${PROJECT_NAME}_tests
    tests/test_core.cpp
    src/Core/Board.cpp
    src/Core/GameEngine.cpp
    src/Core/Player.cpp
    src/AI/AIPlayer.cpp
)
target_include_directories(${PROJECT_NAME}_tests PRIVATE
    include
    include/Core
    include/AI
    include/UI
)
target_link_libraries(${PROJECT_NAME}_tests PRIVATE Qt6::Core Qt6::Concurrent)
set_target_properties(${PROJECT_NAME}_tests PROPERTIES AUTOMOC OFF)
