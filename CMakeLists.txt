cmake_minimum_required(VERSION 3.10.0)
project(NesoraMikomi VERSION 0.1.0 LANGUAGES C CXX)

# Request the required wxWidgets libs
# 第一引数はwxWidgetsのコンポーネントがあるディレクトリを指定
add_subdirectory(/Users/Shared/wxWidgets out/build/cmake/lib/wxWidgets/Makefile EXCLUDE_FROM_ALL)

include(CTest)
enable_testing()

# cファイルをビルド対象に加える
file(GLOB_RECURSE NESORA_MAIN_SOURCES "src/Nesora/*.cpp")
add_library(nesora_main_lib STATIC ${NESORA_MAIN_SOURCES})

file(GLOB_RECURSE NESORA_GUI_SOURCES "src/NesoraStyle/*.cpp")
add_library(nesora_gui_lib STATIC ${NESORA_GUI_SOURCES})
target_link_libraries(nesora_gui_lib wx::net wx::core wx::base wx::aui)

add_executable(Nesora src/NesoraMikomi.cpp)
target_link_libraries(Nesora nesora_main_lib)
target_link_libraries(Nesora nesora_gui_lib)
target_link_libraries(Nesora wx::net wx::core wx::base wx::aui)

# C++の標準規格の指定
target_compile_features(nesora_main_lib PUBLIC cxx_std_17)
target_compile_features(nesora_gui_lib PUBLIC cxx_std_17)
target_compile_features(Nesora PUBLIC cxx_std_17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
