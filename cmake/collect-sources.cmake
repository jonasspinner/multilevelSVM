file(GLOB_RECURSE ALL_HEADERS *.h *.hpp)
list(FILTER ALL_HEADERS EXCLUDE REGEX "${CMAKE_BINARY_DIR}|build|extern")
#list(FILTER ALL_HEADERS INCLUDE REGEX "lib|app")

file(GLOB_RECURSE ALL_SOURCES *.c *.cpp)
list(FILTER ALL_SOURCES EXCLUDE REGEX "${CMAKE_BINARY_DIR}|build|extern")
#list(FILTER ALL_SOURCES INCLUDE REGEX "lib|app")

message(STATUS "Project sources and headers were collected")
