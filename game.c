#include "game.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

static void msgbox_btn_event_cb(lv_event_t * e);
static void msgbox_close_event_cb(lv_event_t * e);


// 卡片背面图片路径 - 只需要这一张图片
#define CARD_BACK_IMG "A:/home/gzj/card_back.bmp"

// 创建游戏界面
lv_obj_t* create_game_screen(lv_obj_t* parent) {
    // 创建游戏主容器
    lv_obj_t* screen = lv_obj_create(parent);
    lv_obj_set_size(screen, 700, 480);
    lv_obj_set_pos(screen, 100, 0);
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN); // 改为垂直布局
    lv_obj_set_flex_align(screen, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    // 设置样式
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_bg_color(&style, lv_color_hex(0xE6E6FA));
    lv_style_set_bg_opa(&style, 255);
    lv_style_set_radius(&style, 10);
    lv_style_set_border_width(&style, 0);
    lv_obj_add_style(screen, &style, 0);
    
    return screen;
}

// 初始化游戏状态
void init_game(GameState* game, GameDifficulty difficulty) {
    game->difficulty = difficulty;
    game->first_card = -1;
    game->second_card = -1;
    game->pairs_found = 0;
    game->moves = 0;
    game->seconds_elapsed = 0;
    
    // 根据难度确定卡片数量
    int card_count;
    switch (difficulty) {
        case GAME_EASY:
            card_count = 16; // 4x4
            break;
        case GAME_MEDIUM:
            card_count = 36; // 6x6
            break;
        case GAME_HARD:
            card_count = 64; // 8x8
            break;
    }
    
    // 生成卡片值对
    srand(time(NULL));
    for (int i = 0; i < card_count; i++) {
        game->card_values[i] = i / 2; // 每对两个相同的值
        game->revealed[i] = 0;
        game->matched[i] = 0;
    }
    
    // 洗牌
    for (int i = 0; i < card_count; i++) {
        int j = rand() % card_count;
        uint8_t temp = game->card_values[i];
        game->card_values[i] = game->card_values[j];
        game->card_values[j] = temp;
    }
    
   // 创建信息显示区域 - 放在顶部
    lv_obj_t* info_panel = lv_obj_create(game->screen);
    lv_obj_set_size(info_panel, 680, 40);
    lv_obj_set_align(info_panel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(info_panel, LV_OBJ_FLAG_SCROLLABLE);
    
    // 创建卡片容器 - 放在信息面板下方
    game->card_container = lv_obj_create(game->screen);
    lv_obj_set_size(game->card_container, 680, 420);
    lv_obj_set_flex_flow(game->card_container, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(game->card_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(game->card_container, LV_OBJ_FLAG_SCROLLABLE);

    // 创建步数标签
    game->moves_label = lv_label_create(info_panel);
    lv_label_set_text_fmt(game->moves_label, "step: 0");
    lv_obj_align(game->moves_label, LV_ALIGN_LEFT_MID, 20, 0);
    
    // 创建时间标签
    game->time_label = lv_label_create(info_panel);
    lv_label_set_text_fmt(game->time_label, "time: 00:00");
    lv_obj_align(game->time_label, LV_ALIGN_RIGHT_MID, -20, 0);
    
    // 创建计时器
    game->timer = lv_timer_create(timer_cb, 1000, game);
    
    // 创建卡片
    create_cards(game);
}

// 创建卡片
void create_cards(GameState* game) {
    int card_count;
    int cols;
    
    switch (game->difficulty) {
        case GAME_EASY:
            card_count = 16;
            cols = 4;
            break;
        case GAME_MEDIUM:
            card_count = 36;
            cols = 6;
            break;
        case GAME_HARD:
            card_count = 64;
            cols = 8;
            break;
    }
    
    // 设置网格布局
    static lv_coord_t col_dsc[8] = {70, 70, 70, 70, 70, 70, 70, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[8] = {70, 70, 70, 70, 70, 70, 70, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(game->card_container, col_dsc, row_dsc);
    
    for (int i = 0; i < card_count; i++) {
        // 创建卡片按钮 - 父对象改为card_container
        lv_obj_t* card = lv_button_create(game->card_container);
        lv_obj_set_size(card, 60, 60);
        lv_obj_set_grid_cell(card, LV_GRID_ALIGN_CENTER, i % cols, 1, 
                             LV_GRID_ALIGN_CENTER, i / cols, 1);
        
        // 添加卡片图片（背面）
        lv_obj_t* img = lv_image_create(card);
        lv_image_set_src(img, CARD_BACK_IMG);
        lv_obj_center(img);
        
        // 添加数字标签（正面）- 初始隐藏
        lv_obj_t* label = lv_label_create(card);
        lv_label_set_text_fmt(label, "%d", game->card_values[i]);
        lv_obj_center(label);
        lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0);
        lv_obj_set_style_text_color(label, lv_color_hex(0x000000), 0);
        
        game->labels[i] = label; // 保存标签对象
        
        // 添加事件回调
        lv_obj_add_event_cb(card, card_event_cb, LV_EVENT_CLICKED, game);
        
        game->cards[i] = card;
    }
}

// 卡片点击事件处理
void card_event_cb(lv_event_t* e) {
    lv_obj_t* card = lv_event_get_target(e);
    GameState* game = (GameState*)lv_event_get_user_data(e);
    
    // 找到点击的卡片索引
    int index = -1;
    int card_count = (game->difficulty == GAME_EASY) ? 16 : 
                   (game->difficulty == GAME_MEDIUM) ? 36 : 64;
    
    for (int i = 0; i < card_count; i++) {
        if (game->cards[i] == card) {
            index = i;
            break;
        }
    }
    
    // 如果卡片已匹配或已翻开，忽略点击
    if (index == -1 || game->matched[index] || game->revealed[index]) {
        return;
    }
    
    // 翻开卡片：隐藏背面图片，显示数字标签
    lv_obj_t* img = lv_obj_get_child(card, 0);
    lv_obj_add_flag(img, LV_OBJ_FLAG_HIDDEN);
    
    lv_obj_t* label = game->labels[index];
    lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);
    
    game->revealed[index] = 1;
    
    // 检查是否是第一张翻开的卡片
    if (game->first_card == -1) {
        game->first_card = index;
    } else {
        game->second_card = index;
        game->moves++;
        lv_label_set_text_fmt(game->moves_label, "step: %d", game->moves);
        
        // 检查匹配
        lv_timer_handler();
        usleep(300000); // 短暂延迟让玩家看到第二张卡片
        check_match(game);
    }
}

// 检查卡片是否匹配
void check_match(GameState* game) {
    if (game->card_values[game->first_card] == game->card_values[game->second_card]) {
        // 匹配成功
        game->matched[game->first_card] = 1;
        game->matched[game->second_card] = 1;
        game->pairs_found++;
        
        // 更新卡片样式 - 添加匹配标记
        lv_obj_add_flag(game->cards[game->first_card], LV_OBJ_FLAG_USER_1);
        lv_obj_add_flag(game->cards[game->second_card], LV_OBJ_FLAG_USER_1);
        
        // 设置匹配卡片为绿色
        lv_obj_set_style_bg_color(game->cards[game->first_card], lv_color_hex(0x00FF00), 0);
        lv_obj_set_style_bg_color(game->cards[game->second_card], lv_color_hex(0x00FF00), 0);
        
        // 检查游戏是否完成
        int total_pairs = (game->difficulty == GAME_EASY) ? 8 : 
                         (game->difficulty == GAME_MEDIUM) ? 18 : 32;
        
        if (game->pairs_found == total_pairs) {
            // 游戏胜利
            lv_timer_pause(game->timer);
            
            // 创建消息框 - 使用您自定义的实现
            lv_obj_t* msgbox = lv_msgbox_create(lv_layer_top());
            lv_msgbox_add_title(msgbox, "WIN!!!");
            lv_msgbox_add_text(msgbox, "GAMEOVER!!!");
            
            // 添加确定按钮
            lv_obj_t* btn = lv_msgbox_add_footer_button(msgbox, "YES");
            
            // 添加关闭按钮
            lv_obj_t* close_btn = lv_msgbox_add_close_button(msgbox);
            
            // 设置消息框位置
            lv_obj_center(msgbox);
            
            // 添加按钮事件处理
            lv_obj_add_event_cb(btn, msgbox_btn_event_cb, LV_EVENT_CLICKED, game);
            lv_obj_add_event_cb(close_btn, msgbox_close_event_cb, LV_EVENT_CLICKED, game);
        }
    } else {
        // 不匹配，翻回背面
        game->revealed[game->first_card] = 0;
        game->revealed[game->second_card] = 0;
        
        // 隐藏标签，显示背面图片
        lv_obj_t* img1 = lv_obj_get_child(game->cards[game->first_card], 0);
        lv_obj_clear_flag(img1, LV_OBJ_FLAG_HIDDEN);
        
        lv_obj_t* label1 = game->labels[game->first_card];
        lv_obj_add_flag(label1, LV_OBJ_FLAG_HIDDEN);
        
        lv_obj_t* img2 = lv_obj_get_child(game->cards[game->second_card], 0);
        lv_obj_clear_flag(img2, LV_OBJ_FLAG_HIDDEN);
        
        lv_obj_t* label2 = game->labels[game->second_card];
        lv_obj_add_flag(label2, LV_OBJ_FLAG_HIDDEN);
    }
    
    // 重置选择
    game->first_card = -1;
    game->second_card = -1;
}

// 计时器回调
void timer_cb(lv_timer_t* timer) {
    GameState* game = (GameState*)timer->user_data;
    game->seconds_elapsed++;
    
    int minutes = game->seconds_elapsed / 60;
    int seconds = game->seconds_elapsed % 60;
    
    lv_label_set_text_fmt(game->time_label, "time: %02d:%02d", minutes, seconds);
}

// 重置游戏
void reset_game(GameState* game) {
    // 停止计时器
    if (game->timer) {
        lv_timer_del(game->timer);
        game->timer = NULL;
    }
    
    // 删除所有卡片
    int card_count = (game->difficulty == GAME_EASY) ? 16 : 
                   (game->difficulty == GAME_MEDIUM) ? 36 : 64;
    
    for (int i = 0; i < card_count; i++) {
        if (game->cards[i]) {
            lv_obj_del(game->cards[i]);
            game->cards[i] = NULL;
        }
    }
    
    // 删除信息面板
    lv_obj_clean(game->screen);
    
    // 重新初始化游戏
    init_game(game, game->difficulty);
}

// 消息框按钮事件回调
static void msgbox_btn_event_cb(lv_event_t * e) {
    lv_obj_t * btn = lv_event_get_current_target(e);
    lv_obj_t * footer = lv_obj_get_parent(btn);
    lv_obj_t * msgbox = lv_obj_get_parent(footer);
    GameState * game = (GameState *)lv_event_get_user_data(e);
    
    lv_obj_del(msgbox);  // 删除消息框
    reset_game(game);    // 重置游戏
}

// 关闭按钮事件回调
static void msgbox_close_event_cb(lv_event_t * e) {
    lv_obj_t * btn = lv_event_get_current_target(e);
    lv_obj_t * header = lv_obj_get_parent(btn);
    lv_obj_t * msgbox = lv_obj_get_parent(header);
    GameState * game = (GameState *)lv_event_get_user_data(e);
    
    lv_obj_del(msgbox);  // 删除消息框
    reset_game(game);    // 重置游戏
}