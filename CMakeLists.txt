cmake_minimum_required(VERSION 3.12)
project(SkyMap)

find_package(OpenCV)
include_directories( ${OpenCV_INCLUDE_DIRS} )

set(CMAKE_CXX_STANDARD 14)

#set(OpenCV_DIR D:\\Program Files\\CLion\\SkyMap\\ThirdParty\\opencv-4.0.1 )
#find_package(OpenCV REQUIRED)
#
#message(STATUS "OpenCV library status:")
#message(STATUS "    config: ${OpenCV_DIR}")
#message(STATUS "    version: ${OpenCV_VERSION}")
#message(STATUS "    libraries: ${OpenCV_LIBS}")
#message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

set(SOURCE_FILES main.cpp)
add_executable(SkyMap main.cpp Sky.cpp Sky.h SkyMapMatching.cpp SkyMapMatching.h Star.cpp Star.h
        CSVAdapter.cpp CSVAdapter.h TriangleMatching.cpp TriangleMatching.h MyFunctions.cpp MyFunctions.h extraction.h extraction.cpp)

target_link_libraries(SkyMap ${OpenCV_LIBS})