cmake_minimum_required(VERSION 3.16)
project(p2 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/Ds/include 
  ${CMAKE_CURRENT_SOURCE_DIR}/Ds/externals/include 
  ${CMAKE_CURRENT_SOURCE_DIR}/externals/include 
  ${CMAKE_CURRENT_SOURCE_DIR}/cgdemo
  ${GLFW_INCLUDE_DIRS}   
)

add_executable(p2
  src/Main.cpp
  src/P2Window.cpp
  src/RayCaster.cpp
  src/PBRRenderer.cpp
  src/SceneLoader.cpp
  cgdemo/MainWindow.cpp
  cgdemo/RayTracer.cpp
  cgdemo/SceneWriter.cpp
  cgdemo/Writer.cpp
  cgdemo/reader/AbstractParser.cpp
  cgdemo/reader/Buffer.cpp
  cgdemo/reader/ErrorHandler.cpp
  cgdemo/reader/Expression.cpp
  cgdemo/reader/FileBuffer.cpp
  cgdemo/reader/ReaderBase.cpp
  cgdemo/reader/SceneReader.cpp
  cgdemo/reader/Scope.cpp
)

find_package(OpenGL REQUIRED)

if(WIN32)

  set(PLATFORM_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/windows)

  set_target_properties(p2 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_CURRENT_SOURCE_DIR}
  )

  target_link_libraries(p2 PRIVATE
    ${PLATFORM_LIB_DIR}/cg.lib
    ${PLATFORM_LIB_DIR}/gl3w.lib
    ${PLATFORM_LIB_DIR}/glfw3.lib
    OpenGL::GL
  )

else()

  set(PLATFORM_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux)

  find_package(glfw3 REQUIRED)

  set_target_properties(p2 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_CURRENT_SOURCE_DIR}
  )

  target_link_libraries(p2 PRIVATE
    ${PLATFORM_LIB_DIR}/libcg.a
    ${PLATFORM_LIB_DIR}/libgl3w.a
    glfw
    OpenGL::GL
  )

endif()
