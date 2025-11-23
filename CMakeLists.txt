cmake_minimum_required(VERSION 3.16)
project(p2 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/Ds/include 
  ${CMAKE_CURRENT_SOURCE_DIR}/Ds/externals/include 
  ${CMAKE_CURRENT_SOURCE_DIR}/externals/include 
  ${GLFW_INCLUDE_DIRS}   
)

add_executable(p2
  src/Main.cpp
  src/MainWindow.cpp
  src/RayCaster.cpp
  src/PBRRenderer.cpp
  src/SceneLoader.cpp
)

find_package(OpenGL REQUIRED)

if(WIN32)

  set(PLATFORM_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/windows)

  target_link_libraries(p2 PRIVATE
    ${PLATFORM_LIB_DIR}/cg.lib
    ${PLATFORM_LIB_DIR}/gl3w.lib
    ${PLATFORM_LIB_DIR}/glfw3.lib
    OpenGL::GL
  )

else()

  set(PLATFORM_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux)

  find_package(glfw3 REQUIRED)

  target_link_libraries(p2 PRIVATE
    ${PLATFORM_LIB_DIR}/libcg.a
    ${PLATFORM_LIB_DIR}/libgl3w.a
    glfw
    OpenGL::GL
  )

endif()
