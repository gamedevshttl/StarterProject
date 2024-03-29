cmake_minimum_required(VERSION 3.10)

project(StarterProgect)

find_package(OpenGL REQUIRED)

include_directories(
#	external/glfw/include
#	external/glm
	src
)

set(SOURCE_CODE src/main.cpp
				src/Dependencies/imgui_impl_glfw.cpp
				src/Dependencies/imgui_impl_glfw.h
				src/Dependencies/imgui_impl_opengl3.cpp
				src/Dependencies/imgui_impl_opengl3.h
				src/glCore/gl_core_4_4.h
				src/glCore/gl_core_4_4.c
				src/Render/Shader.h
				src/Render/Shader.cpp
				src/Render/Texture.h
				src/Render/Texture.cpp
				src/Render/SpriteRenderer.h
				src/Render/SpriteRenderer.cpp
				src/ResourceManager/ResourceManager.h
				src/ResourceManager/ResourceManager.cpp
				src/ResourceManager/stb_image.h
				src/Scene/Node.h
				src/Scene/Node.cpp
				src/Scene/Scene.h
				src/Scene/Scene.cpp
				src/Scene/SceneBase.h
				src/Scene/SceneBase.cpp
				src/Scene/SceneLight.h
				src/Scene/SceneLight.cpp
				src/Scene/SceneTexture.h
				src/Scene/SceneTexture.cpp
				src/Scene/SceneFBO.h
				src/Scene/SceneFBO.cpp
				src/Camera/Camera.h
				src/Camera/Camera.cpp
				src/Application.h
				src/Application.cpp
				src/Gameplay/Game.h
				src/Gameplay/Game.cpp
				src/Object/drawable.h
				src/Object/teapot.h
				src/Object/torus.h
				src/Object/trianglemesh.h
				src/Object/teapot.cpp
				src/Object/teapotdata.h
				src/Object/torus.cpp
				src/Object/trianglemesh.cpp
				src/Object/objmesh.h
				src/Object/objmesh.cpp
				src/Object/aabb.h
				src/Object/utils.h
				src/Object/cube.cpp
				src/Object/cube.h
				src/Object/skybox.cpp
				src/Object/skybox.h
				src/shader/sprite.vs
				src/shader/sprite.fs
				src/shader/lamp.vs
				src/shader/lamp.fs
				src/shader/discard_sprite.vs
				src/shader/discard_sprite.fs
				src/shader/flat.frag
				src/shader/flat.vert
				src/shader/twoside.frag
				src/shader/twoside.vert
				)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
	set(CMAKE_CXX_STANDARD 17)
endif()

set(CONAN_DISABLE_CHECK_COMPILER TRUE)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(StarterProgect ${SOURCE_CODE})

#add_subdirectory(external/glfw)

foreach(source IN LISTS SOURCE_CODE)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach()

#target_link_libraries(StarterProgect glfw  ${GLFW_LIBRARIES} ${OPENGL_LIBRARY})

conan_target_link_libraries(StarterProgect)