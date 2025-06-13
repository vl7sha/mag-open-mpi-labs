cmake_minimum_required(VERSION 3.12)
project(MPI_OpenMP_Programs CXX)

# Установить компилятор вручную
set(CMAKE_CXX_COMPILER mpic++)

# Включить OpenMP
find_package(OpenMP REQUIRED)

# Стандарт C++
set(CMAKE_CXX_STANDARD 17)

# Добавить исполняемые файлы

add_executable(Program1 lab1/SearchMaxValueForColumnInMatrix.cpp)
target_link_libraries(Program1 OpenMP::OpenMP_CXX)

add_executable(Program2 lab2/SearchMaxValueForColumnInMatrixWithoutOmpFor.cpp)
target_link_libraries(Program2 OpenMP::OpenMP_CXX)

add_executable(Program3 lab3/SimpleCalculation.cpp)
target_link_libraries(Program3 OpenMP::OpenMP_CXX)

add_executable(Program4 lab4/EffictiveSimpleCalculation.cpp)
target_link_libraries(Program4 OpenMP::OpenMP_CXX)

add_executable(Program5 lab5/WorkForString.cpp)
target_link_libraries(Program5 OpenMP::OpenMP_CXX)
