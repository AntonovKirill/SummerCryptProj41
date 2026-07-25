# Минимальная требуемая версия CMake
cmake_minimum_required(VERSION 3.16)

# Название проекта
project(SummerCryptProj41 VERSION 1.0)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем исполняемый файл
add_executable(crypto_solver 
    main.cpp
    equations_system.cpp
    equations_system.hpp
    linearizing_set.cpp
    linearizing_set.hpp
    search_tree.cpp
    search_tree.hpp
)

# Подключаем библиотеку cmath (математическая библиотека очень вероятно понадобится)
target_link_libraries(crypto_solver PRIVATE m)
