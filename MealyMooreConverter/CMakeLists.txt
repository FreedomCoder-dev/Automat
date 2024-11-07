cmake_minimum_required(VERSION 3.10)

#имя проекта
project(MealyMooreConverter)

#стандарт с++
set(CMAKE_CXX_STANDARD 11)

#добавление исходных файлов
set(SOURCE_FILES
	
	src/MealyMooreConverter.cpp
	src/MealyMooreConverter.h
)

#добавление исполняемого файла
add_executable(MealyMooreConverter ${SOURCE_FILES})