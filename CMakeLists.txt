cmake_minimum_required(VERSION 2.8.12)
project(PoeFastPricer)

add_definitions("-std=c++11")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable (poefastpricer 
    src/main.cpp 
    src/requestType/overview.h 
    src/requestType/overview.cpp
    src/requestType/result_listing.h
    src/requestType/result_listing.cpp
    src/request.h
    src/request.cpp
    src/clipboard.h
    src/clipboard.cpp
    src/item_parser.h
    src/item_parser.cpp
    src/nuklear.h
    src/stb_image.h
    src/items/base_item.h
    src/items/unique_item.h
    src/items/unique_item.cpp
    src/items/currency_item.h
    src/items/currency_item.cpp
    src/items/normal_item.h
    src/items/normal_item.cpp
)
target_link_libraries(poefastpricer ${CONAN_LIBS})