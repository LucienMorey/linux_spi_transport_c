cmake_minimum_required(VERSION 3.12)

project(TRANS_SPI LANGUAGES C)

add_library(trans_spi STATIC src/trans_spi.c)

target_include_directories(trans_spi PUBLIC include)
target_include_directories(trans_spi PRIVATE src)