#ifndef __MUSICPIC_H__
#define __MUSICPIC_H__
#include "DoubleCircularLinkedList.h"
#include<pthread.h>
typedef struct newtype
{
    lv_obj_t * bmp;
    lv_obj_t * pic;
    lv_obj_t * qqmusic;
    lv_obj_t * st;
    lv_obj_t * game;
    lv_obj_t * video;

    BiNode ** n;
    BiNode ** m;
    BiNode ** v;

    // 添加链表头指针（用于存储音乐/图片/视频列表）
    BiHead * music_list;  // 新增
    BiHead * pic_list;    // 新增
    BiHead * video_list;  // 新增


    lv_timer_t * timer;
    int musicflag;

    int video_fd;  // 管道文件描述符
    pid_t video_pid; // 视频播放进程PID
    int video_playing; // 视频播放状态
     
}NewType;



extern NewType app_data;

BiHead * create_music_list();
BiHead * create_pic_list();
BiHead * create_video_list();

lv_obj_t * create_video_b_screen();
lv_obj_t * create_start_screen(lv_obj_t * win);
lv_obj_t * create_pic_screen(lv_obj_t * win);
lv_obj_t * create_qqmusic_screen(lv_obj_t * win);
lv_obj_t * create_video_m_screen(lv_obj_t * win);
lv_obj_t * create_video_screen(lv_obj_t * win);


lv_obj_t * Create_Button1(lv_obj_t * win);
lv_obj_t * Create_Button2(lv_obj_t * win);
lv_obj_t * Create_Button3(lv_obj_t * win);
lv_obj_t * Create_Button4(lv_obj_t * win);
lv_obj_t * Create_Button5(lv_obj_t * win);
lv_obj_t * Create_Button6(lv_obj_t * win);
lv_obj_t * Create_Button7(lv_obj_t * win);
lv_obj_t * Create_Button8(lv_obj_t * win);
lv_obj_t * Create_Button9(lv_obj_t * win);
lv_obj_t * Create_Button10(lv_obj_t * win);
lv_obj_t * Create_Button11(lv_obj_t * win);
lv_obj_t * Create_Button12(lv_obj_t * win);
lv_obj_t * Create_Button13(lv_obj_t * win);

lv_obj_t * create_lab1(lv_obj_t * but1);
lv_obj_t * create_lab2(lv_obj_t * but2);
lv_obj_t * create_lab3(lv_obj_t * but3);
lv_obj_t * create_lab4(lv_obj_t * but4);
lv_obj_t * create_lab5(lv_obj_t * but5);
lv_obj_t * create_lab6(lv_obj_t * but6);
lv_obj_t * create_lab7(lv_obj_t * but7);
lv_obj_t * create_lab8(lv_obj_t * but8);
lv_obj_t * create_lab9(lv_obj_t * but9);
lv_obj_t * create_lab10(lv_obj_t * but10);
lv_obj_t * create_lab11(lv_obj_t * but11);
lv_obj_t * create_lab12(lv_obj_t * but12);
lv_obj_t * create_lab13(lv_obj_t * but13);


void kill_music_cb(lv_event_t * e);
void start_but1_cb(lv_event_t * e);
static void timer1_cb(lv_timer_t * timer);
void CirStart_cb(lv_event_t * e);
void prev_pic_cb(lv_event_t * e);
void next_pic_cb(lv_event_t * e);
static void pic_gesture_cb(lv_event_t *e);
static void pic_click_cb(lv_event_t *e);


void prev_music_cb(lv_event_t * e);
void next_music_cb(lv_event_t * e);
void stop_music_cb(lv_event_t * e);
void Start_music_cb(lv_event_t * e);
void setup_music_controls(lv_obj_t *music_screen, NewType *data);



lv_obj_t * Create_VideoButton(lv_obj_t * win, int x, int y, const char* text);
void setup_video_controls(lv_obj_t * video_screen, NewType * data);

void play_video_cb(lv_event_t * e);
void stop_video_cb(lv_event_t * e);
void pause_video_cb(lv_event_t * e);
void next_video_cb(lv_event_t * e);
void prev_video_cb(lv_event_t * e);
void volumeadd_video_cb(lv_event_t * e);
void volumere_video_cb(lv_event_t * e);
void seekf_video_cb(lv_event_t * e);
void seeks_video_cb(lv_event_t * e);

void setup_music_controls(lv_obj_t *music_screen, NewType *data);
void back_btn_cb(lv_event_t *e);
   

#endif


