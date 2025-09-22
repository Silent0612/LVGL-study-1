#include "app_menu.h"
#include "musicpic.h"
// 声明外部全局变量（从main.c中共享）
// 确保包含 NewType 定义的头文件
extern NewType app_data;
// 补充缺失的LVGL符号定义（LVGL 9.0可能未内置的符号）
#ifndef LV_SYMBOL_GAME
#define LV_SYMBOL_GAME "\uF11B"  // 游戏符号
#endif

#ifndef LV_SYMBOL_CLOCK
#define LV_SYMBOL_CLOCK "\uF017"  // 时钟符号（Unicode时钟图标）
#endif

#ifndef LV_SYMBOL_VIDEO
#define LV_SYMBOL_VIDEO "\uF03D"  // 视频符号
#endif

static AppState current_app = APP_MAIN_MENU;
static lv_obj_t *current_screen = NULL;



// 游戏状态变量
static GameState game_state;

// 应用图标结构
typedef struct {
    const char *name;
    const char *icon;
    lv_color_t color;
    AppState app;
} AppIcon;

// 替换原apps数组初始化，使用LVGL的颜色宏或直接赋值
static const AppIcon apps[] = {
    {"Memory Game", LV_SYMBOL_GAME, LV_COLOR_MAKE(0x9B, 0x59, 0xB6), APP_GAME},
    {"Music Player", LV_SYMBOL_AUDIO, LV_COLOR_MAKE(0x34, 0x98, 0xDB), APP_MUSIC},
    {"Photo Viewer", LV_SYMBOL_IMAGE, LV_COLOR_MAKE(0xE7, 0x4C, 0x3C), APP_PHOTO},
    {"Network Time", LV_SYMBOL_CLOCK, LV_COLOR_MAKE(0x2E, 0xCC, 0x71), APP_NETWORK_TIME},
    {"FTP Client", LV_SYMBOL_DOWNLOAD, LV_COLOR_MAKE(0xF3, 0x9C, 0x12), APP_FTP_CLIENT},
    {"Video Player", LV_SYMBOL_VIDEO, LV_COLOR_MAKE(0x1A, 0xBC, 0x9C), APP_VIDEO}
};
static void app_btn_cb(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    AppState app = (AppState)(intptr_t)lv_event_get_user_data(e);
    switch_app(app);
}

void back_btn_cb(lv_event_t *e) {
    switch_app(APP_MAIN_MENU);
}

void create_main_menu(lv_obj_t *parent) {
    // 创建网格容器
    lv_obj_t *grid = lv_obj_create(parent);
    lv_obj_set_size(grid, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(grid, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(grid, LV_OBJ_FLAG_SCROLLABLE);
    
    // 设置背景（LVGL 9中样式使用方式保持兼容）
    static lv_style_t bg_style;
    lv_style_init(&bg_style);
    lv_style_set_bg_color(&bg_style, lv_color_hex(0x1A1A2E));
    lv_style_set_bg_opa(&bg_style, LV_OPA_COVER);
    lv_obj_add_style(grid, &bg_style, 0);
    
    // 标题
    lv_obj_t *title = lv_label_create(grid);
    lv_label_set_text(title, "Multi-Function App Center");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_width(title, lv_pct(100));
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
    
    // 创建应用按钮
    for (int i = 0; i < sizeof(apps) / sizeof(apps[0]); i++) {
        lv_obj_t *btn = lv_btn_create(grid);
        lv_obj_set_size(btn, 150, 150);
        
        // 设置按钮样式
        static lv_style_t btn_style;
        lv_style_init(&btn_style);
        lv_style_set_bg_color(&btn_style, apps[i].color);
        lv_style_set_bg_opa(&btn_style, LV_OPA_COVER);
        lv_style_set_radius(&btn_style, 20);
        lv_obj_add_style(btn, &btn_style, 0);
        
        // 图标
        lv_obj_t *icon = lv_label_create(btn);
        lv_label_set_text(icon, apps[i].icon);
        lv_obj_set_style_text_font(icon, &lv_font_montserrat_36, 0);
        lv_obj_set_style_text_color(icon, lv_color_white(), 0);
        lv_obj_align(icon, LV_ALIGN_TOP_MID, 0, 20);
        
        // 应用名称
        lv_obj_t *label = lv_label_create(btn);
        lv_label_set_text(label, apps[i].name);
        lv_obj_set_style_text_color(label, lv_color_white(), 0);
        lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, -20);
        
        // 添加点击事件
        lv_obj_add_event_cb(btn, app_btn_cb, LV_EVENT_CLICKED, (void *)(intptr_t)apps[i].app);
    }
}

void switch_app(AppState new_app) {
    // 关闭当前应用
    if (current_screen) {
        lv_obj_del(current_screen);
        current_screen = NULL;
    }
    
    // 关闭特定应用资源
    if (current_app == APP_NETWORK_TIME) {
        network_time_close();
    } else if (current_app == APP_FTP_CLIENT) {
        ftp_client_close();
    } else if (current_app == APP_MUSIC) {
        // 停止音乐播放
        system("pkill -9 madplay");
    } else if (current_app == APP_VIDEO) {
        // 停止视频播放
        system("pkill -9 mplayer");
    }
    
    current_app = new_app;
    
    // 创建新应用
    lv_obj_t *parent = lv_scr_act();
    
    if (new_app == APP_MAIN_MENU) {
        create_main_menu(parent);
        return;
    }
    
    // 创建应用容器
    current_screen = lv_obj_create(parent);
    lv_obj_set_size(current_screen, lv_pct(100), lv_pct(100));
    
    // 添加返回按钮
    lv_obj_t *back_btn = lv_btn_create(current_screen);
    lv_obj_set_size(back_btn, 60, 40);
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, LV_SYMBOL_LEFT);
    lv_obj_center(back_label);
    lv_obj_add_event_cb(back_btn, back_btn_cb, LV_EVENT_CLICKED, NULL);
    
    switch (new_app) {
    case APP_GAME:
        game_state.screen = create_game_screen(current_screen);
        init_game(&game_state, GAME_EASY);
        break;
    case APP_MUSIC:
        app_data.qqmusic = create_qqmusic_screen(current_screen);
        // 确保链表正确初始化
        if (!app_data.music_list) {
            app_data.music_list = create_music_list();
        }
        // 确保当前节点有效
        if (app_data.music_list && app_data.music_list->first) {
            app_data.m = &(app_data.music_list->first);
            setup_music_controls(app_data.qqmusic, &app_data);
        } else {
            // 添加错误处理
            lv_obj_t *err_label = lv_label_create(app_data.qqmusic);
            lv_label_set_text(err_label, "No music files found!");
            lv_obj_center(err_label);
        }
        break;
    // 修改APP_PHOTO分支：
    case APP_PHOTO:
        app_data.pic = create_pic_screen(current_screen);
        if (app_data.pic_list && app_data.pic_list->first) {
            app_data.n = &(app_data.pic_list->first);
            if (app_data.n && *app_data.n) {
                lv_obj_t *img = lv_img_create(app_data.pic);
                lv_img_set_src(img, (*app_data.n)->data);
                lv_obj_center(img);
                app_data.bmp = img;
            }
        }
        break;
    case APP_VIDEO:
        app_data.video = create_video_b_screen(current_screen);
        setup_video_controls(app_data.video, &app_data);
        break;
    case APP_NETWORK_TIME:
        network_time_init(current_screen);
        break;
    case APP_FTP_CLIENT:
        ftp_client_init(current_screen);
        break;
    default:
        break;
    }
}