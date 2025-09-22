cmake_minimum_required(VERSION 3.10)


#设置系统名与框架名
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
#设置交叉编译工具链的路径 
set(CMAKE_C_COMPILER /usr/local/arm/gcc-linaro-5.5.0-2017.10-x86_64_arm-linux-gnueabi/bin/arm-linux-gcc)
set(CMAKE_CXX_COMPILER /usr/local/arm/gcc-linaro-5.5.0-2017.10-x86_64_arm-linux-gnueabi/bin/arm-linux-g++)

project(lvgl)

set(CMAKE_C_STANDARD 99)#C99 # lvgl officially support C99 and above
set(CMAKE_CXX_STANDARD 17)#C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(lvgl)
target_include_directories(lvgl PUBLIC ${PROJECT_SOURCE_DIR})

add_executable(main main.c mouse_cursor_icon.c  myself_src/musicpic.c myself_src/DoubleCircularLinkedList.c myself_src/game.c myself_src/video.c myself_src/network_time.c myself_src/ftp_client.c myself_src/app_menu.c)

target_link_libraries(main lvgl lvgl::examples lvgl::demos lvgl::thorvg ${SDL2_LIBRARIES} m pthread)
add_custom_target (run COMMAND ${EXECUTABLE_OUTPUT_PATH}/main DEPENDS main)

