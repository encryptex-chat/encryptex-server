cmake_minimum_required(VERSION 3.22.1)

project(encryptex-server)

set(CMAKE_CXX_STANDARD 23)

set(ENCRYPTEX_SERVER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
find_package(Boost REQUIRED)
find_package(spdlog REQUIRED)

file(GLOB ENCRYPTEX_SERVER_SOURCE ${ENCRYPTEX_SERVER_SOURCE_DIR}/*.cpp)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
    add_subdirectory("encryptex-common-lib")
    add_executable(${PROJECT_NAME} ${ENCRYPTEX_SERVER_SOURCE})
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wall")
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} spdlog::spdlog encryptex-common-lib)
endif()

