cmake_minimum_required(VERSION 3.26)
set(MAJOR_VERSION 1)

set(PROJECT_NAME "casino_bot")

project(${PROJECT_NAME} VERSION 1.0.0 DESCRIPTION "Casino bot for Fiesta Online")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Executable output path
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

if (DEBUG)
    add_executable(${PROJECT_NAME})
    target_compile_definitions(${PROJECT_NAME} PUBLIC CB_DEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
    target_compile_options(${PROJECT_NAME} PUBLIC "/Z7")
else()
    add_executable(${PROJECT_NAME} WIN32)
endif()

#######################
# Include directories #
#######################

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
##################
# Link libraries #
##################

target_link_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

########
# MISC #
########

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH "x64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    # 32 bits
    set(ARCH "x86")
endif()
message("Compiling for ${ARCH}")

add_subdirectory(src)
