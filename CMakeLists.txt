cmake_minimum_required(VERSION 3.22)
project(ina226)





set(INA226_SRC ina226.c)
set(INCLUDE_DIRS .)

add_library(ina226 STATIC
        ${INA226_SRC}
        )

target_include_directories(ina226 PUBLIC ${INCLUDE_DIRS})
target_link_libraries(ina226)


