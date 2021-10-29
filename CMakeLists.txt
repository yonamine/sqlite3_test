cmake_minimum_required(VERSION 3.15)

project(sqlite3_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Debug")
set(BIN "sqlite3_test")

set(SOURCE_FILES "main.cpp"
                 #"sqlite3-dev/sqlite3.c"
                 #"sqlite3-dev/sqlite3.h"
                 #"sqlite3-dev/sqlite3ext.h"
)

find_package(unofficial-sqlite3 CONFIG REQUIRED)

add_executable(${BIN} ${SOURCE_FILES})

target_link_libraries(${BIN} PRIVATE
                      unofficial::sqlite3::sqlite3)

#find_package(SQLiteCpp CONFIG REQUIRED)
#target_link_libraries(main PRIVATE SQLiteCpp)

#target_include_directories(sqlite3_test
#                           PUBLIC
#                           "${CMAKE_CURRENT_SOURCE_DIR}/sqlite3-dev/")

message("CMAKE_BINARY_DIR         = ${CMAKE_BINARY_DIR}")
message("CMAKE_SOURCE_DIR         = ${CMAKE_SOURCE_DIR}")
message("CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}")
message("CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")
message("CMAKE_INSTALL_PREFIX     = ${CMAKE_INSTALL_PREFIX}")
set(CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ)
message("CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS     = ${CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS}")


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/city.csv
                                                    $<TARGET_FILE_DIR:${PROJECT_NAME}>)


#file(COPY ${CMAKE_CURRENT_BINARY_DIR}/Debug/sqlite3_test.exe DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


