#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "myself_src/app_menu.h"
#include "myself_src/musicpic.h"
#include "myself_src/DoubleCircularLinkedList.h"
#include "myself_src/game.h"

// 定义全局共享数据
NewType app_data;

// 初始化共享数据
static void init_app_data() {
    memset(&app_data, 0, sizeof(NewType));
    
    // 创建媒体文件链表
    app_data.music_list = create_music_list();
    app_data.pic_list = create_pic_list();
    app_data.video_list = create_video_list();
    
    // 安全初始化指针
    if (app_data.music_list && app_data.music_list->first) {
        app_data.m = &(app_data.music_list->first);
    }
    
    if (app_data.pic_list && app_data.pic_list->first) {
        app_data.n = &(app_data.pic_list->first);
    }
    
    if (app_data.video_list && app_data.video_list->first) {
        app_data.v = &(app_data.video_list->first);
    }
    
    // 初始化状态变量
    app_data.musicflag = 0;
    app_data.video_fd = -1;
    app_data.video_pid = -1;
    app_data.video_playing = 0;
}

int main(void) {
    // LVGL初始化
    lv_init();
    
    // 显示设备配置
    lv_display_t * disp = lv_linux_fbdev_create();
    lv_linux_fbdev_set_file(disp, "/dev/fb0");

    // 输入设备配置
    lv_indev_t * indev = lv_evdev_create(LV_INDEV_TYPE_POINTER, "/dev/input/event0");
    
    // 初始化应用共享数据
    init_app_data();

    // 创建主菜单
    switch_app(APP_MAIN_MENU);

    // LVGL主循环
    while(1) {
        lv_timer_handler();
        usleep(5000);
    }

    return 0;
}