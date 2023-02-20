cmake_minimum_required(VERSION 3.12)

project(SPI_TEST LANGUAGES C)

add_subdirectory(libs)

add_executable(spi_test src/main.c)
target_link_libraries(spi_test trans_spi)