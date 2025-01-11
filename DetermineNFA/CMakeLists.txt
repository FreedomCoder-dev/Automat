cmake_minimum_required(VERSION 3.10)

#имя проекта
project(DetermineNFA)

#стандарт с++
set(CMAKE_CXX_STANDARD 11)

#добавление исходных файлов
set(SOURCE_FILES
	
	src/DetermineNFA.cpp
	src/DetermineNFA.h
)

#добавление исполняемого файла
add_executable(DetermineNFA ${SOURCE_FILES})
