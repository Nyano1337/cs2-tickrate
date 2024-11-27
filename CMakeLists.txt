cmake_minimum_required(VERSION 3.22)

project(cs2tickrate)

include("shared.cmake")

set(TICKRATE_SRC "./src")

include_directories(
	${TICKRATE_SRC}
	./vendor/libmem/include
	./vendor/libmodule
	./vendor/nlohmann
)

if(WIN32)
	file(GLOB TICKRATE_LIBS 
			"./vendor/libmem/lib/Release/*.lib"
	)
else()
	file(GLOB TICKRATE_LIBS 
			"./vendor/libmem/lib/*.a"
	)
endif()

set(LIBMODULE_DIR "./vendor/libmodule")
file(GLOB_RECURSE LIBMODULE_SOURCES "${LIBMODULE_DIR}/module.cpp" "${LIBMODULE_DIR}/*.h")
if(WIN32)
	list(APPEND LIBMODULE_SOURCES "${LIBMODULE_DIR}/module_windows.cpp")
else()
	list(APPEND LIBMODULE_SOURCES "${LIBMODULE_DIR}/module_linux.cpp")
endif()

file(GLOB_RECURSE TICKRATE_SOURCES "${TICKRATE_SRC}/*.cpp" "${TICKRATE_SRC}/*.h")

add_definitions(-DHV_STATICLIB -DHAVE_MEMOVERRIDE_NEW_DELETE)

add_library(${PROJECT_NAME} SHARED ${SDK_CONVAR} ${LIBMODULE_SOURCES} ${TICKRATE_SOURCES})

if(WIN32)
	target_link_options(${PROJECT_NAME} PRIVATE "ntdll.lib")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${TICKRATE_LIBS})

target_precompile_headers(${PROJECT_NAME} PRIVATE "${TICKRATE_SRC}/pch.h")

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

set(OUT_DIR "NULL" CACHE PATH "SurfTimer binary directory for post-build file copy")

if (EXISTS ${OUT_DIR})
	add_custom_command(
		TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
			$<TARGET_FILE:${PROJECT_NAME}>
			${OUT_DIR}
	)
endif()

