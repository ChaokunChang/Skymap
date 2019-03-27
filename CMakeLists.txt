cmake_minimum_required(VERSION 3.13)
project(SkyMap)

set(CMAKE_CXX_STANDARD 14)

#set(OpenCV_DIR D:/libs/opencv-build)
#find_package(OpenCV REQUIRED)
#message(STATUS "OpenCV library status:")
#message(STATUS "    config: ${OpenCV_DIR}")
#message(STATUS "    version: ${OpenCV_VERSION}")
#message(STATUS "    libraries: ${OpenCV_LIBS}")
#message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

add_executable(SkyMap main.cpp Sky.cpp Sky.h SkyMapMatching.cpp SkyMapMatching.h Star.cpp Star.h
        CSVAdapter.cpp CSVAdapter.h TriangleMatching.cpp TriangleMatching.h MyFunctions.cpp MyFunctions.h
        )

#target_link_libraries(SkyMap ${OpenCV_LIBS})