cmake_minimum_required(VERSION 3.10)

#имя проекта
project(Minimize)

#стандарт с++
set(CMAKE_CXX_STANDARD 11)

#добавление исходных файлов
set(SOURCE_FILES
	
	src/Minimize.cpp
	src/Minimize.h
)

#добавление исполняемого файла
add_executable(Minimize ${SOURCE_FILES})
