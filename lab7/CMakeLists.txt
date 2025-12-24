cmake_minimum_required(VERSION 3.15)
project(lab7)

set(CMAKE_CXX_STANDARD 17)

# Основная асинхронная программа
add_executable(lab7_async
    main.cpp
    src/npc/npc.cpp
    src/npc/factory.cpp
    src/npc/npc_types.cpp
    src/visitor/battle_visitor.cpp    
    src/observer/observer.cpp
    src/editor/editor.cpp
)

target_include_directories(lab7_async PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(lab7_async pthread) # Необходимо для std::thread

# Программа из 6 лабораторной (для сравнения/демонстрации)
add_executable(lab6
    main.cpp # Оригинальный main из 6 лабы
    src/npc/npc.cpp
    src/npc/factory.cpp
    src/npc/npc_types.cpp
    src/visitor/battle_visitor.cpp    
    src/observer/observer.cpp
    src/editor/editor.cpp
)

target_include_directories(lab6 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Тесты (опционально, можно адаптировать)
# find_package(GTest REQUIRED)
# add_executable(tests
#     tests/tests.cpp
#     src/npc/npc.cpp
#     src/npc/factory.cpp
#     src/npc/npc_types.cpp
#     src/visitor/battle_visitor.cpp    
#     src/observer/observer.cpp
#     src/editor/editor.cpp
# )
# target_include_directories(tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# target_link_libraries(tests GTest::gtest GTest::gtest_main)