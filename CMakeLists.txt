# Минимальная требуемая версия CMake
cmake_minimum_required(VERSION 3.16)

# Название проекта
project(SummerCrypProj41 VERSION 1.0)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем исполняемый файл
add_executable(crypto_solver 
    main.cpp
    equations_system.cpp
    equations_system.hpp
    linear_system.cpp
    linear_system.hpp
    linearizing_set.cpp
    linearizing_set.hpp
    quadratic_system.cpp
    quadratic_system.hpp
    search_tree.cpp
    search_tree.hpp
)

# Подключаем библиотеку cmath (математическая библиотека очень верорятно понадобиться)
target_link_libraries(crypto_solver PRIVATE m)
