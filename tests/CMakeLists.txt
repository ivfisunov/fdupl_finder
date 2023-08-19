find_package(GTest REQUIRED)

set(TARGET_NAME ${PROJECT_NAME}_tests)

add_custom_target(copy_fixtures
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/fixtures ${CMAKE_CURRENT_BINARY_DIR}/fixtures
)

add_executable(${TARGET_NAME} fdupl_tests.cpp)

add_dependencies(${TARGET_NAME} copy_fixtures)

target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/include
)

target_link_libraries(${TARGET_NAME}
    gtest
    gtest_main
    pthread
    fdupl_lib
)