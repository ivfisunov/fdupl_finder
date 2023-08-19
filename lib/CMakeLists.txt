project(fdupl_lib LANGUAGES CXX)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SOURCE_FILES)

find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS filesystem REQUIRED)

add_library(${PROJECT_NAME} STATIC
        ${SOURCE_FILES}
    )

target_link_libraries(${PROJECT_NAME} PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
        Boost::filesystem
    )

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${OPENSSL_INCLUDE_DIR}
    )
