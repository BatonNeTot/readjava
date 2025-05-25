cmake_minimum_required (VERSION 3.8)

project ("readjava")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROJECT_TARGET "readjava")

add_subdirectory ("src")

file(GLOB_RECURSE PROJECT_HEADERS CONFIGURE_DEPENDS include/*.h)

add_executable (${PROJECT_TARGET} ${PROJECT_SOURCES} ${PROJECT_HEADERS})
set_property(TARGET ${PROJECT_TARGET} PROPERTY C_STANDARD 11)

set_target_properties(${PROJECT_TARGET}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>"
    LINKER_LANGUAGE C
)

target_compile_definitions(${PROJECT_TARGET} PRIVATE _AMD64_)

if(MSVC)
    target_compile_definitions(${PROJECT_TARGET} PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

target_include_directories(${PROJECT_TARGET} PUBLIC "include")
target_include_directories(${PROJECT_TARGET} PRIVATE "src") 