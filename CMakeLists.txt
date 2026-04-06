cmake_minimum_required(VERSION 3.17)

project(SDLDemo)

file(GLOB SRCS
    "${CMAKE_CURRENT_LIST_DIR}/src/*.cc"
    "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/*.h")

file(GLOB_RECURSE IMGUI_SRCS
    "${CMAKE_CURRENT_LIST_DIR}/src/imgui/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/imgui/*.h")


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)

add_executable(${PROJECT_NAME} ${SRCS} ${IMGUI_SRCS})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/SDL/include)
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/SDL/lib/x64/SDL3.lib)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/imgui)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/cef)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/cef/libs)
target_link_libraries(${PROJECT_NAME} PRIVATE libcef_dll_wrapper libcef)

add_custom_command(TARGET SDLDemo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
        "${CMAKE_SOURCE_DIR}/src/cef/bin"
        "$<TARGET_FILE_DIR:SDLDemo>/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
        "${CMAKE_SOURCE_DIR}/SDL/bin"
        "$<TARGET_FILE_DIR:SDLDemo>/"
    COMMENT "Copying assets files to output directory"
)