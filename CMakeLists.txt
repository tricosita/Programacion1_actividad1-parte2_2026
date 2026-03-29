cmake_minimum_required(VERSION 3.15)
project(mi_juego CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    # ------------------------------
    # Carpeta target para ejecutable, assets y scripts
    # ------------------------------
    set(TARGET_DIR "${CMAKE_BINARY_DIR}/release")
    file(MAKE_DIRECTORY ${TARGET_DIR})
else()
    set(TARGET_DIR "${CMAKE_BINARY_DIR}/debug")
    file(MAKE_DIRECTORY ${TARGET_DIR})
endif()

# ------------------------------
# Descargar Raylib si no está presente
# ------------------------------
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)
FetchContent_MakeAvailable(raylib)

# ------------------------------
# Buscar todos los .cpp y .h dentro de src/
# ------------------------------
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.cpp
    src/*.h
)

# ------------------------------
# Crear ejecutable
# ------------------------------
add_executable(${PROJECT_NAME} ${SOURCES})

# Linkear Raylib
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Copiar ejecutable al target después de build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}"
        "${TARGET_DIR}/"
)

# Copiar assets al target después de build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/assets"
        "${TARGET_DIR}/assets"
)

# ------------------------------
# Crear script de ejecución
# ------------------------------
# UNIX: run.sh
if(UNIX)
    set(RUN_SH "${TARGET_DIR}/run.sh")
    file(WRITE ${RUN_SH} "#!/bin/bash\nDIR=\"\$(cd \"\$(dirname \"\$0\")\" && pwd)\"\n\"\$DIR/${PROJECT_NAME}\" \"\$DIR/assets\"\n")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND chmod +x "${RUN_SH}"
    )
endif()

# Windows: run.bat
if(WIN32)
    set(RUN_BAT "${TARGET_DIR}/run.bat")
    file(WRITE ${RUN_BAT} "@echo off\r\nset DIR=%~dp0\r\n\"%DIR%\\${PROJECT_NAME}.exe\" \"%DIR%\\assets\"\r\npause\r\n")
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E remove "${CMAKE_BINARY_DIR}/${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}")