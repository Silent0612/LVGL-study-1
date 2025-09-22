#ifndef __GAME_H__
#define __GAME_H__

#include "lvgl/lvgl.h"

// 游戏难度级别
typedef enum {
    GAME_EASY,      // 4x4 卡片
    GAME_MEDIUM,    // 6x6 卡片
    GAME_HARD       // 8x8 卡片
} GameDifficulty;

// 游戏状态结构
typedef struct {
    lv_obj_t* screen;       // 游戏界面
    lv_obj_t* card_container;
    lv_obj_t* cards[64];    // 卡片对象数组 (最大支持8x8)
    lv_obj_t* labels[64];   // 卡片上的标签数组
    uint8_t card_values[64];// 卡片对应值 (0-31)
    uint8_t revealed[64];   // 卡片是否翻开 (0/1)
    uint8_t matched[64];    // 卡片是否匹配 (0/1)
    int first_card;         // 第一张翻开的卡片索引 (-1表示没有)
    int second_card;        // 第二张翻开的卡片索引
    int pairs_found;        // 已找到的配对数量
    int moves;              // 移动步数
    GameDifficulty difficulty; // 当前难度
    lv_timer_t* timer;      // 游戏计时器
    lv_obj_t* moves_label;  // 步数显示标签
    lv_obj_t* time_label;   // 时间显示标签
    int seconds_elapsed;    // 经过的秒数
} GameState;

// 函数声明
lv_obj_t* create_game_screen(lv_obj_t* parent);
void init_game(GameState* game, GameDifficulty difficulty);
void create_cards(GameState* game);
void card_event_cb(lv_event_t* e);
void check_match(GameState* game);
void reset_game(GameState* game);
void update_game_ui(GameState* game);
void timer_cb(lv_timer_t* timer);

#endif // __GAME_H__