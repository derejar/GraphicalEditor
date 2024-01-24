cmake_minimum_required(VERSION 3.24)
project(GraphicalEditor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(wxBUILD_SHARED OFF)

add_subdirectory(libs/wxWidgets)
add_subdirectory(libs/glm)
add_subdirectory(libs/glew)

set(CMAKE_CXX_FLAGS "-Wall -static-libstdc++ -static-libgcc")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(SRC
    src/mainwindow.cpp
    src/canvas.cpp
    src/shaderprogram.cpp
    src/line.cpp
    src/logger.cpp
)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} glu32 gdi32 opengl32 wx::base wx::core wx::gl wx::stc glm::glm libglew_static)

if(WIN32 AND ${CMAKE_BUILD_TYPE} MATCHES Release)
	add_custom_command(
	TARGET ${PROJECT_NAME}
	POST_BUILD
	COMMAND "mt.exe" -manifest "${wxWidgets_ROOT_DIR}/include/wx/msw/wx_dpi_aware.manifest" -outputresource:"$<TARGET_FILE:${PROJECT_NAME}>"
	COMMENT "Adding manifest"
	)
endif()
