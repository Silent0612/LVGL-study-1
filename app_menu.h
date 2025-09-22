#ifndef __APP_MENU_H__
#define __APP_MENU_H__

#include "lvgl/lvgl.h"
#include "game.h"
#include "musicpic.h"
#include "network_time.h"
#include "ftp_client.h"



typedef enum {
    APP_MAIN_MENU,
    APP_GAME,
    APP_MUSIC,
    APP_PHOTO,
    APP_NETWORK_TIME,
    APP_FTP_CLIENT,
    APP_VIDEO
} AppState;

void create_main_menu(lv_obj_t *parent);
void switch_app(AppState new_app);

#endif