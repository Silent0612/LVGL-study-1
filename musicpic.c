/*
    创建两个链表，一个音乐链表，一个图片链表
    读取目录文件添加到链表里面去
    
    通过双向循环链表实现上一张，下一张，循环播放，暂停播放。。。。。。循环播放时需要放音乐
    
    点击功能只有下一张、
    可以通过滑动来实现上下张切换

    循环播放单独一个按钮，配暂停按钮

    拓展，可以指定一张照片开启一个记忆匹配小游戏
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "../lvgl/lvgl.h"
#include "../lvgl/demos/lv_demos.h"
#include "musicpic.h"
#include "DoubleCircularLinkedList.h"
#include "../lv_myself_font.h"


int flag = -1;
//创建音乐链表
BiHead * create_music_list()
{   
    BiHead * h = Create_Head();
    BiNode * node = NULL;
    //切换到当前路径记录当前路径的值
    chdir("/home/gzj");
    char path[256];
    getcwd(path,256);
    //并寻找当前路径下的音乐文件合并为绝对路径,均为.mp3
    DIR * dirp = opendir("/home/gzj");
    if(dirp == NULL)
	{
		perror("opendir error");
	}
    struct dirent * fl = NULL;//寻找目录项
    char * target = "mp3";
    while(1)
    {   
        fl = readdir(dirp);
        if(fl == NULL)
		{
			perror("readdir ");
			break;
		}
        char * p = fl->d_name;
        char * p2 = "..";
        if(p[0] == '.' || !strcmp(p,p2))
        {
            continue;
        }
        while(*p != '\0')
        {   
            if(*p == '.')
            {
                p++;
                char * s = p;
                if(!strcmp(s,target))
                {   
                    char d[256];
                    sprintf(d,"%s/%s",path,fl->d_name);
                    printf("music-d-%s\n",d);
                    node = malloc(sizeof(BiNode));
                    node->data = malloc(256 * sizeof(char));
                    strcpy(node->data , d);
                    node->next = NULL;
                    node->prev = NULL;

                    //如果链表中没有一个数据结点,则node将作为第一个,也是最后一个
                    if(h->first == NULL)
                    {
                        h->first = node;
                        h->last = node;
                        node->next = node;
                        node->prev = node;
                        h->num++;
                    }
                    else 
                    {
                        //"尾插法"
                        node->prev = h->last;
                        node->next = h->first; 
                        h->last->next = node;
                        h->first->prev = node;
                        h->last = node;
                        h->num++;
                    }
                    break;
                }
                break;
            }
            else
            {
               p++; 
            }
        }
    }
    return h;
}



//创建图片链表
BiHead * create_pic_list()
{
    BiHead * h = Create_Head();
    BiNode * node = NULL;
    //切换到当前路径记录当前路径的值
    chdir("/home/gzj");
    char path[256];
    getcwd(path,256);
    //并寻找当前路径下的音乐文件合并为绝对路径,均为.mp3
    DIR * dirp = opendir("/home/gzj");
    if(dirp == NULL)
	{
		perror("opendir error");
	}
    struct dirent * fl = NULL;//寻找目录项
    char * target1 = "bmp";
    char * target2 = "gif";
    while(1)
    {   
        fl = readdir(dirp);
        if(fl == NULL)
		{
			perror("readdir ");
			break;
		}
        char * p = fl->d_name;
        char * p2 = "..";
        if(p[0] == '.' || !strcmp(p,p2))
        {
            continue;
        }
        while(*p != '\0')
        {   
            if(*p == '.')
            {
                p++;
                char * s = p;
                if(!strcmp(s,target1) || !strcmp(s,target2))
                {   
                    char d[256];
                    char a[3] = "A:";
                    sprintf(d,"%s%s/%s",a,path,fl->d_name);
                    printf("pic-d-%s\n",d);
                    node = malloc(sizeof(BiNode));
                    node->data = malloc(256 * sizeof(char));
                    strcpy(node->data , d);
                    node->next = NULL;
                    node->prev = NULL;

                    //如果链表中没有一个数据结点,则node将作为第一个,也是最后一个
                    if(h->first == NULL)
                    {
                        h->first = node;
                        h->last = node;
                        node->next = node;
                        node->prev = node;
                        h->num++;
                    }
                    else 
                    {
                        //"尾插法"
                        node->prev = h->last;
                        node->next = h->first; 
                        h->last->next = node;
                        h->first->prev = node;
                        h->last = node;
                        h->num++;
                    }
                    break;
                }
                break;
            }
            else
            {
               p++; 
            }
        }
    }
    closedir(dirp);
    return h;
}


//创建视频链表
BiHead * create_video_list()
{   
    BiHead * h = Create_Head();
    BiNode * node = NULL;
    //切换到当前路径记录当前路径的值
    chdir("/home/gzj");
    char path[256];
    getcwd(path,256);
    //并寻找当前路径下的视频文件合并为绝对路径,均为.mp4
    DIR * dirp = opendir("/home/gzj");
    if(dirp == NULL)
	{
		perror("opendir error");
	}
    struct dirent * fl = NULL;//寻找目录项
    char * target = "mp4";
    while(1)
    {   
        fl = readdir(dirp);
        if(fl == NULL)
		{
			perror("readdir ");
			break;
		}
        char * p = fl->d_name;
        char * p2 = "..";
        if(p[0] == '.' || !strcmp(p,p2))
        {
            continue;
        }
        while(*p != '\0')
        {   
            if(*p == '.')
            {
                p++;
                char * s = p;
                if(!strcmp(s,target))
                {   
                    char d[256];
                    sprintf(d,"%s",fl->d_name);
                    printf("video-d-%s\n",d);
                    node = malloc(sizeof(BiNode));
                    node->data = malloc(256 * sizeof(char));
                    strcpy(node->data , d);
                    node->next = NULL;
                    node->prev = NULL;

                    //如果链表中没有一个数据结点,则node将作为第一个,也是最后一个
                    if(h->first == NULL)
                    {
                        h->first = node;
                        h->last = node;
                        node->next = node;
                        node->prev = node;
                        h->num++;
                    }
                    else 
                    {
                        //"尾插法"
                        node->prev = h->last;
                        node->next = h->first; 
                        h->last->next = node;
                        h->first->prev = node;
                        h->last = node;
                        h->num++;
                    }
                    break;
                }
                break;
            }
            else
            {
               p++; 
            }
        }
    }
    return h;
}


//创建视频按钮活动窗口
lv_obj_t * create_video_b_screen()
{   
    //在活动屏幕上,创建一个窗口
    lv_obj_t * win = lv_obj_create(lv_screen_active());
    //指定win大小
    lv_obj_set_size(win,700,80);
    //显示位置
    lv_obj_set_pos(win,100,400);
    //
    lv_obj_remove_flag(win,LV_OBJ_FLAG_SCROLLABLE);

    //设计一个样式对象
    static lv_style_t style;
    //初始化样式
    lv_style_init(&style);

    //添加样式属性

    //背景
    //无边框
    lv_style_set_bg_color(&style, lv_color_hex(0xFFFFFF));
	lv_style_set_bg_opa( &style, 255);


    //将样式绑定到窗口对象上
    lv_obj_add_style(win,&style,LV_STATE_DEFAULT);

    return win;
}

//设置开始屏幕
lv_obj_t * create_start_screen(lv_obj_t * win)
{   
    //在活动屏幕上,创建一个窗口
    lv_obj_t * pic = lv_obj_create(win);
    //指定win大小
    lv_obj_set_size(pic,700,480);
    //设置窗口在活动屏幕显示位置 
	lv_obj_set_pos(pic , 100, 0);

    //设计一个样式对象
    static lv_style_t style;
    //初始化样式
    lv_style_init(&style);

    //添加样式属性
    //背景
    lv_style_set_bg_color(&style, lv_color_hex(0xFFFFFF));
	lv_style_set_bg_opa( &style, 255);

    //边框
    lv_style_set_border_color(&style,lv_color_hex(0x00FF00));
    lv_style_set_radius(&style, 10);
	lv_style_set_border_opa( &style, 255);
	lv_style_set_border_width(&style, 5);
	lv_style_set_border_side(&style, LV_BORDER_SIDE_FULL);


    //将样式绑定到窗口对象上
    lv_obj_add_style(pic,&style,LV_STATE_DEFAULT);

    return pic;
}

//设置视频播放主界面
lv_obj_t * create_video_m_screen(lv_obj_t * win)
{   
    //在活动屏幕上,创建一个窗口
    lv_obj_t * pic = lv_obj_create(win);
    //指定win大小
    lv_obj_set_size(pic,700,400);
    //设置窗口在活动屏幕显示位置 
	lv_obj_set_pos(pic , 100, 0);
    //设计一个样式对象
    static lv_style_t style;
    //初始化样式
    lv_style_init(&style);

    //添加样式属性
    //背景
    lv_style_set_bg_color(&style, lv_color_hex(0xFFFFFF));
	lv_style_set_bg_opa( &style, 255);


    //将样式绑定到窗口对象上
    lv_obj_add_style(pic,&style,LV_STATE_DEFAULT);

    return pic;
}

//设置视频播放窗口
lv_obj_t * create_video_screen(lv_obj_t * win)
{   
    //在活动屏幕上,创建一个窗口
    lv_obj_t * pic = lv_obj_create(win);
    //指定win大小
    lv_obj_set_size(pic,700,400);
    //设置窗口在活动屏幕显示位置 
	lv_obj_set_pos(pic , 100, 0);

    //设计一个样式对象
    static lv_style_t style;
    //初始化样式
    lv_style_init(&style);

    //添加样式属性
    //背景
    lv_style_set_bg_color(&style, lv_color_hex(0xFFE384));
	lv_style_set_bg_opa( &style, 255);

    //边框
    lv_style_set_border_color(&style,lv_color_hex(0x00FF00));
    lv_style_set_radius(&style, 10);
	lv_style_set_border_opa( &style, 255);
	lv_style_set_border_width(&style, 5);
	lv_style_set_border_side(&style, LV_BORDER_SIDE_FULL);


    //将样式绑定到窗口对象上
    lv_obj_add_style(pic,&style,LV_STATE_DEFAULT);

    return pic;
}


//创建放置照片的活动窗口
lv_obj_t * create_pic_screen(lv_obj_t * win) {
    lv_obj_t *pic_screen = lv_obj_create(win);
    lv_obj_set_size(pic_screen, lv_pct(100), lv_pct(100));
    

    // 添加返回按钮
    lv_obj_t *back_btn = lv_btn_create(pic_screen);
    lv_obj_set_size(back_btn, 60, 40);
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, LV_SYMBOL_LEFT);
    lv_obj_center(back_label);
    lv_obj_add_event_cb(back_btn, back_btn_cb, LV_EVENT_CLICKED, NULL);

    // 添加事件监听
    lv_obj_add_event_cb(pic_screen, pic_gesture_cb, LV_EVENT_GESTURE, &app_data);
    lv_obj_add_event_cb(pic_screen, pic_click_cb, LV_EVENT_CLICKED, &app_data);
    
    // 上一张按钮
    lv_obj_t *prev_btn = lv_btn_create(pic_screen);
    lv_obj_set_size(prev_btn, 60, 60);
    lv_obj_align(prev_btn, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_t *prev_label = lv_label_create(prev_btn);
    lv_label_set_text(prev_label, LV_SYMBOL_LEFT);
    lv_obj_center(prev_label);
    lv_obj_add_event_cb(prev_btn, prev_pic_cb, LV_EVENT_CLICKED, &app_data);
    
    // 下一张按钮
    lv_obj_t *next_btn = lv_btn_create(pic_screen);
    lv_obj_set_size(next_btn, 60, 60);
    lv_obj_align(next_btn, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_t *next_label = lv_label_create(next_btn);
    lv_label_set_text(next_label, LV_SYMBOL_RIGHT);
    lv_obj_center(next_label);
    lv_obj_add_event_cb(next_btn, next_pic_cb, LV_EVENT_CLICKED, &app_data);
    
    // 初始化显示第一张图片
    if (app_data.n && *app_data.n) {
        app_data.bmp = lv_img_create(pic_screen);
        lv_img_set_src(app_data.bmp, (*app_data.n)->data);
        lv_obj_center(app_data.bmp);
    }
    
    return pic_screen;
}

//创建放置音乐播放器界面的活动窗口
lv_obj_t * create_qqmusic_screen(lv_obj_t * win)
{   
    //在活动屏幕上,创建一个窗口
    lv_obj_t * pic = lv_obj_create(win);

    // 设置背景图片
    lv_image_set_src(pic , "A:/home/gzj/qqmusic.bmp");
    lv_obj_set_style_bg_opa(pic, LV_OPA_COVER, 0);
    //指定win大小
    lv_obj_set_size(pic,700,480);
    //设置窗口在活动屏幕显示位置 
	lv_obj_set_pos(pic , 100, 0);

    //设计一个样式对象
    static lv_style_t style;
    //初始化样式
    lv_style_init(&style);

    //添加样式属性
    //背景
    lv_style_set_bg_color(&style, lv_color_hex(0xEEA98B));
	lv_style_set_bg_opa( &style, 255);

    //边框
    lv_style_set_border_color(&style,lv_color_hex(0x00FF00));
    lv_style_set_radius(&style, 10);
	lv_style_set_border_opa( &style, 255);
	lv_style_set_border_width(&style, 5);
	lv_style_set_border_side(&style, LV_BORDER_SIDE_FULL);


    //将样式绑定到窗口对象上
    lv_obj_add_style(pic,&style,LV_STATE_DEFAULT);

    return pic;
}

//在活动窗口上创建开始按钮
lv_obj_t * Create_Button1(lv_obj_t * win)
{
    lv_obj_t * but = lv_button_create(win);
    lv_obj_set_size(but,100,40);
    lv_obj_set_pos(but,0,0);
    return but;
}





//在活动窗口上创建循环按钮
lv_obj_t * Create_Button2(lv_obj_t * win)
{
    lv_obj_t * but = lv_button_create(win);
    lv_obj_set_size(but,100,40);
    lv_obj_set_pos(but,0,50);
    return but;
}

//音乐播放器按钮
lv_obj_t * Create_Button3(lv_obj_t * win)
{
    lv_obj_t * but = lv_button_create(win);
    lv_obj_set_size(but,100,40);
    lv_obj_set_pos(but,0,100);
    return but;
}

//杀死音乐线程按钮
lv_obj_t * Create_Button4(lv_obj_t * win)
{
    lv_obj_t * but = lv_button_create(win);
    lv_obj_set_size(but,100,40);
    lv_obj_set_pos(but,0,150);
    return but;
}

//恢复屏幕
lv_obj_t * Create_Button5(lv_obj_t * win)
{
    lv_obj_t * but = lv_button_create(win);
    lv_obj_set_size(but,100,40);
    lv_obj_set_pos(but,0,200);
    return but;
}

//上一首
lv_obj_t * Create_Button6(lv_obj_t * win)
{
    lv_obj_t * but = lv_button_create(win);
    lv_obj_set_size(but,100,40);
    lv_obj_set_pos(but,100,400);
    return but;
}

//下一首
lv_obj_t * Create_Button7(lv_obj_t * win)
{
    lv_obj_t * but = lv_button_create(win);
    lv_obj_set_size(but,100,40);
    lv_obj_set_pos(but,300,400);
    return but;
}

//暂停
lv_obj_t * Create_Button8(lv_obj_t * win)
{
    lv_obj_t * but = lv_button_create(win);
    lv_obj_set_size(but,100,40);
    lv_obj_set_pos(but,500,400);
    return but;
}

//开始
lv_obj_t * Create_Button9(lv_obj_t * win)
{
    lv_obj_t * but = lv_button_create(win);
    lv_obj_set_size(but,100,40);
    lv_obj_set_pos(but,300,300);
    return but;
}

//记忆小游戏按钮
lv_obj_t * Create_Button10(lv_obj_t * win)
{
    lv_obj_t * but = lv_button_create(win);
    lv_obj_set_size(but,100,40);
    lv_obj_set_pos(but,0,250);
    return but;
}

//视频播放器按钮
lv_obj_t * Create_Button11(lv_obj_t * win)
{
    lv_obj_t * but = lv_button_create(win);
    lv_obj_set_size(but,100,40);
    lv_obj_set_pos(but,0,300);
    return but;
}

//获取网络时间按钮
lv_obj_t * Create_Button12(lv_obj_t * win)
{
    lv_obj_t * but = lv_button_create(win);
    lv_obj_set_size(but,100,40);
    lv_obj_set_pos(but,0,350);
    return but;
}

//ftp按钮
lv_obj_t * Create_Button13(lv_obj_t * win)
{
    lv_obj_t * but = lv_button_create(win);
    lv_obj_set_size(but,100,40);
    lv_obj_set_pos(but,0,350);
    return but;
}

//开始按钮绑定标签
lv_obj_t * create_lab1(lv_obj_t * but1)
{
    //再到窗口上创建一个标签
	lv_obj_t * lab = lv_label_create(but1);
	lv_obj_set_size(lab ,50, 50);
	lv_obj_set_align(lab ,  LV_ALIGN_CENTER);

    //再为标签设计一个样式
    static lv_style_t style;
    lv_style_init(&style);

    //添加样式属性 
	//背景
	lv_style_set_bg_color(&style, lv_color_hex(0x0000FF));
	lv_style_set_bg_opa( &style, 255);


	//将样式绑定到窗口对象上 
	lv_obj_add_style(lab , &style, LV_STATE_DEFAULT);


    return lab;
}


//循环按钮绑定标签
lv_obj_t * create_lab2(lv_obj_t * but2)
{
    //再到窗口上创建一个标签
	lv_obj_t * lab = lv_label_create(but2);
	lv_obj_set_size(lab ,50, 50);
	lv_obj_set_align(lab ,  LV_ALIGN_CENTER);

    //再为标签设计一个样式
    static lv_style_t style;
    lv_style_init(&style);

    //添加样式属性 
	//背景
	lv_style_set_bg_color(&style, lv_color_hex(0x0000FF));
	lv_style_set_bg_opa( &style, 255);

	lv_style_set_text_font(&style ,&lv_myself_font);

    //设置文本的颜色 
	lv_style_set_text_color(&style, lv_color_hex(0x000000));

	//将样式绑定到窗口对象上 
	lv_obj_add_style(lab , &style, LV_STATE_DEFAULT);

	lv_label_set_text(lab , "循环播放");

    return lab;
}

lv_obj_t * create_lab3(lv_obj_t * but3)
{
    //再到窗口上创建一个标签
	lv_obj_t * lab = lv_label_create(but3);
	lv_obj_set_size(lab ,50, 50);
	lv_obj_set_align(lab ,  LV_ALIGN_CENTER);

    //再为标签设计一个样式
    static lv_style_t style;
    lv_style_init(&style);

    //添加样式属性 
	//背景
	lv_style_set_bg_color(&style, lv_color_hex(0x0000FF));
	lv_style_set_bg_opa( &style, 255);

	lv_style_set_text_font(&style , &lv_myself_font);

    //设置文本的颜色 
	lv_style_set_text_color(&style, lv_color_hex(0x000000));

	//将样式绑定到窗口对象上 
	lv_obj_add_style(lab , &style, LV_STATE_DEFAULT);

	lv_label_set_text(lab , "音乐播放器");

    return lab;
}


lv_obj_t * create_lab4(lv_obj_t * but4)
{
    //再到窗口上创建一个标签
	lv_obj_t * lab = lv_label_create(but4);
	lv_obj_set_size(lab ,50, 50);
	lv_obj_set_align(lab ,  LV_ALIGN_CENTER);

    //再为标签设计一个样式
    static lv_style_t style;
    lv_style_init(&style);

    //添加样式属性 
	//背景
	lv_style_set_bg_color(&style, lv_color_hex(0x0000FF));
	lv_style_set_bg_opa( &style, 255);

	lv_style_set_text_font(&style , &lv_myself_font);

    //设置文本的颜色 
	lv_style_set_text_color(&style, lv_color_hex(0x000000));

	//将样式绑定到窗口对象上 
	lv_obj_add_style(lab , &style, LV_STATE_DEFAULT);

	lv_label_set_text(lab , "杀死音乐");

    return lab;
}


lv_obj_t * create_lab5(lv_obj_t * but5)
{
    //再到窗口上创建一个标签
	lv_obj_t * lab = lv_label_create(but5);
	lv_obj_set_size(lab ,50, 50);
	lv_obj_set_align(lab ,  LV_ALIGN_CENTER);

    //再为标签设计一个样式
    static lv_style_t style;
    lv_style_init(&style);

    //添加样式属性 
	//背景
	lv_style_set_bg_color(&style, lv_color_hex(0x0000FF));
	lv_style_set_bg_opa( &style, 255);

	lv_style_set_text_font(&style , &lv_myself_font);

    //设置文本的颜色 
	lv_style_set_text_color(&style, lv_color_hex(0x000000));

	//将样式绑定到窗口对象上 
	lv_obj_add_style(lab , &style, LV_STATE_DEFAULT);

	lv_label_set_text(lab , "恢复按钮");

    return lab;
}

//上一曲
lv_obj_t * create_lab6(lv_obj_t * but6)
{
    //再到窗口上创建一个标签
	lv_obj_t * lab = lv_label_create(but6);
	lv_obj_set_size(lab ,50, 50);
	lv_obj_set_align(lab ,  LV_ALIGN_CENTER);

    //再为标签设计一个样式
    static lv_style_t style;
    lv_style_init(&style);

    //添加样式属性 
	//背景
	lv_style_set_bg_color(&style, lv_color_hex(0x0000FF));
	lv_style_set_bg_opa( &style, 255);

	lv_style_set_text_font(&style , &lv_myself_font);

    //设置文本的颜色 
	lv_style_set_text_color(&style, lv_color_hex(0x000000));

	//将样式绑定到窗口对象上 
	lv_obj_add_style(lab , &style, LV_STATE_DEFAULT);

	lv_label_set_text(lab , "上一曲");

    return lab;
}

//下一曲
lv_obj_t * create_lab7(lv_obj_t * but7)
{
    //再到窗口上创建一个标签
	lv_obj_t * lab = lv_label_create(but7);
	lv_obj_set_size(lab ,50, 50);
	lv_obj_set_align(lab ,  LV_ALIGN_CENTER);

    //再为标签设计一个样式
    static lv_style_t style;
    lv_style_init(&style);

    //添加样式属性 
	//背景
	lv_style_set_bg_color(&style, lv_color_hex(0x0000FF));
	lv_style_set_bg_opa( &style, 255);

	lv_style_set_text_font(&style ,&lv_myself_font);

    //设置文本的颜色 
	lv_style_set_text_color(&style, lv_color_hex(0x000000));

	//将样式绑定到窗口对象上 
	lv_obj_add_style(lab , &style, LV_STATE_DEFAULT);

	lv_label_set_text(lab , "下一曲");

    return lab;
}


lv_obj_t * create_lab8(lv_obj_t * but8)
{
    //再到窗口上创建一个标签
	lv_obj_t * lab = lv_label_create(but8);
	lv_obj_set_size(lab ,50, 50);
	lv_obj_set_align(lab ,  LV_ALIGN_CENTER);

    //再为标签设计一个样式
    static lv_style_t style;
    lv_style_init(&style);

    //添加样式属性 
	//背景
	lv_style_set_bg_color(&style, lv_color_hex(0x0000FF));
	lv_style_set_bg_opa( &style, 255);

	lv_style_set_text_font(&style , &lv_myself_font);

    //设置文本的颜色 
	lv_style_set_text_color(&style, lv_color_hex(0x000000));

	//将样式绑定到窗口对象上 
	lv_obj_add_style(lab , &style, LV_STATE_DEFAULT);

	lv_label_set_text(lab , "暂停");

    return lab;
}

lv_obj_t * create_lab9(lv_obj_t * but9)
{
    //再到窗口上创建一个标签
	lv_obj_t * lab = lv_label_create(but9);
	lv_obj_set_size(lab ,50, 50);
	lv_obj_set_align(lab ,  LV_ALIGN_CENTER);

    //再为标签设计一个样式
    static lv_style_t style;
    lv_style_init(&style);

    //添加样式属性 
	//背景
	lv_style_set_bg_color(&style, lv_color_hex(0x0000FF));
	lv_style_set_bg_opa( &style, 255);

	lv_style_set_text_font(&style , &lv_myself_font);

    //设置文本的颜色 
	lv_style_set_text_color(&style, lv_color_hex(0x000000));

	//将样式绑定到窗口对象上 
	lv_obj_add_style(lab , &style, LV_STATE_DEFAULT);

	lv_label_set_text(lab , "开始");

    return lab;
}

lv_obj_t * create_lab10(lv_obj_t * but10)
{
    //再到窗口上创建一个标签
	lv_obj_t * lab = lv_label_create(but10);
	lv_obj_set_size(lab ,50, 50);
	lv_obj_set_align(lab ,  LV_ALIGN_CENTER);

    //再为标签设计一个样式
    static lv_style_t style;
    lv_style_init(&style);

    //添加样式属性 
	//背景
	lv_style_set_bg_color(&style, lv_color_hex(0x0000FF));
	lv_style_set_bg_opa( &style, 255);

	lv_style_set_text_font(&style , &lv_myself_font);

    //设置文本的颜色 
	lv_style_set_text_color(&style, lv_color_hex(0x000000));

	//将样式绑定到窗口对象上 
	lv_obj_add_style(lab , &style, LV_STATE_DEFAULT);

	lv_label_set_text(lab , "记忆小游戏");

    return lab;
}

lv_obj_t * create_lab11(lv_obj_t * but11)
{
    //再到窗口上创建一个标签
	lv_obj_t * lab = lv_label_create(but11);
	lv_obj_set_size(lab ,50, 50);
	lv_obj_set_align(lab ,  LV_ALIGN_CENTER);

    //再为标签设计一个样式
    static lv_style_t style;
    lv_style_init(&style);

    //添加样式属性 
	//背景
	lv_style_set_bg_color(&style, lv_color_hex(0x0000FF));
	lv_style_set_bg_opa( &style, 255);

	lv_style_set_text_font(&style , &lv_myself_font);

    //设置文本的颜色 
	lv_style_set_text_color(&style, lv_color_hex(0x000000));

	//将样式绑定到窗口对象上 
	lv_obj_add_style(lab , &style, LV_STATE_DEFAULT);

	lv_label_set_text(lab , "视频播放器");

    return lab;
}

lv_obj_t * create_lab12(lv_obj_t * but12)
{
    //再到窗口上创建一个标签
	lv_obj_t * lab = lv_label_create(but12);
	lv_obj_set_size(lab ,50, 50);
	lv_obj_set_align(lab ,  LV_ALIGN_CENTER);

    //再为标签设计一个样式
    static lv_style_t style;
    lv_style_init(&style);

    //添加样式属性 
	//背景
	lv_style_set_bg_color(&style, lv_color_hex(0x0000FF));
	lv_style_set_bg_opa( &style, 255);

	lv_style_set_text_font(&style , &lv_myself_font);

    //设置文本的颜色 
	lv_style_set_text_color(&style, lv_color_hex(0x000000));

	//将样式绑定到窗口对象上 
	lv_obj_add_style(lab , &style, LV_STATE_DEFAULT);

	lv_label_set_text(lab , "同步获取网络时间");

    return lab;
}

lv_obj_t * create_lab13(lv_obj_t * but13)
{
    //再到窗口上创建一个标签
	lv_obj_t * lab = lv_label_create(but13);
	lv_obj_set_size(lab ,50, 50);
	lv_obj_set_align(lab ,  LV_ALIGN_CENTER);

    //再为标签设计一个样式
    static lv_style_t style;
    lv_style_init(&style);

    //添加样式属性 
	//背景
	lv_style_set_bg_color(&style, lv_color_hex(0x0000FF));
	lv_style_set_bg_opa( &style, 255);

	lv_style_set_text_font(&style , &lv_myself_font);

    //设置文本的颜色 
	lv_style_set_text_color(&style, lv_color_hex(0x000000));

	//将样式绑定到窗口对象上 
	lv_obj_add_style(lab , &style, LV_STATE_DEFAULT);

	lv_label_set_text(lab , "视频播放器");

    return lab;
}




void kill_music_cb(lv_event_t * e)
{
    system("pkill -9 madplay");
}



void prev_pic_cb(lv_event_t * e) {
    NewType *data = (NewType *)lv_event_get_user_data(e);
    if (!data || !data->n || !*data->n || !(*data->n)->prev) return;
    
    *data->n = (*data->n)->prev;
    if (data->bmp) {
        lv_img_set_src(data->bmp, (*data->n)->data);
    }
}

void next_pic_cb(lv_event_t * e) {
    NewType *data = (NewType *)lv_event_get_user_data(e);
    if (!data || !data->n || !*data->n || !(*data->n)->next) return;
    
    *data->n = (*data->n)->next;
    if (data->bmp) {
        lv_img_set_src(data->bmp, (*data->n)->data);
    }
}

// 滑动检测回调
static void pic_gesture_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code != LV_EVENT_GESTURE) return;
    
    NewType *data = (NewType *)lv_event_get_user_data(e);
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    
    if (dir == LV_DIR_LEFT) {
        next_pic_cb(e);  // 左滑下一张
    } else if (dir == LV_DIR_RIGHT) {
        prev_pic_cb(e);  // 右滑上一张
    }
}

// 点击空白区域切换图片
static void pic_click_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code != LV_EVENT_CLICKED) return;
    
    lv_obj_t *obj = lv_event_get_target(e);
    lv_indev_t *indev = lv_indev_get_act();
lv_point_t p;
lv_indev_get_point(indev, &p);
lv_coord_t x = p.x;
    lv_coord_t w = lv_obj_get_width(obj);
    
    // 点击左侧切换到上一张，右侧切换到下一张
    if (x < w / 2) {
        prev_pic_cb(e);
    } else {
        next_pic_cb(e);
    }
}

//开始按钮
void start_but1_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
	if(code == LV_EVENT_CLICKED)
	{
        NewType * data = (NewType *) lv_event_get_user_data(e);
        BiNode ** p = (BiNode **)data->n;
        lv_obj_t * bmp = data->bmp;
        lv_obj_t * qqmusic = data->qqmusic;
        lv_obj_t * pic = data->pic;
	    lv_obj_t * game = data->game;
        lv_obj_t * st = data->st;

        lv_obj_add_flag(st, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_add_flag(game, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_add_flag( qqmusic, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_remove_flag( pic, LV_OBJ_FLAG_HIDDEN);
        lv_image_set_src(bmp , (*p)->data);
    }
}




//循环播放图片flag用来在主函数创建音乐播放线程控制音乐的播放停止
// 定时器回调函数 - 用于切换图片
static void timer1_cb(lv_timer_t * timer)
{
    NewType * data = (NewType *)timer->user_data;
    BiNode ** p = (BiNode **)data->n;
    lv_obj_t * bmp = data->bmp;
    
    // 切换到下一张图片
    (*p) = (*p)->next;
    lv_image_set_src(bmp, (*p)->data);
}

// 循环播放按钮回调
void CirStart_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code != LV_EVENT_CLICKED) return;

    NewType * data = (NewType *)lv_event_get_user_data(e);
    
    if(flag == -1) // 初始状态
    {
        // 显示第一张图片
        lv_image_set_src(data->bmp, (*(data->n))->data);
        
        // 创建定时器（每1秒切换一次）
        data->timer = lv_timer_create(timer1_cb, 1000, data);
        flag = 1; // 标记为播放状态
    }
    else if(flag == 1) // 正在播放
    {
        // 暂停定时器
        lv_timer_pause(data->timer);
        flag = 0; // 标记为暂停状态
    }
    else if(flag == 0) // 已暂停
    {
        // 恢复定时器
        lv_timer_resume(data->timer);
        flag = 1; // 标记为播放状态
    }
}


void prev_music_cb(lv_event_t * e)
{
    NewType * data = (NewType *)lv_event_get_user_data(e);
    BiNode ** p = (BiNode **)data->m;
    (*p) = (*p)->prev;
    system("pkill -CONT madplay");
    data->musicflag = 1;
    
}
void next_music_cb(lv_event_t * e)
{
    NewType * data = (NewType *)lv_event_get_user_data(e);
    BiNode ** p = (BiNode **)data->m;
    (*p) = (*p)->next;
    system("pkill -CONT madplay");
    data->musicflag = 1;
}

void stop_music_cb(lv_event_t * e)
{
    NewType * data = (NewType *)lv_event_get_user_data(e);
    BiNode ** p = (BiNode **)data->m;
    if(data->musicflag == 1)
    {
        system("pkill -STOP madplay");
        data->musicflag = 0;
    }
    else
    {
        system("pkill -CONT madplay");
        data->musicflag = 1;
    }
    
}
void Start_music_cb(lv_event_t * e)
{
    NewType * data = (NewType *)lv_event_get_user_data(e);
    BiNode ** p = (BiNode **)data->m;
    char cmd[256] = {0};
	sprintf(cmd,"madplay -Q %s &",(*p)->data);
    system(cmd);
    data->musicflag = 1; 
}



// 创建视频控制按钮
lv_obj_t * Create_VideoButton(lv_obj_t * win, int x, int y, const char* text) {
    lv_obj_t * btn = lv_button_create(win);
    lv_obj_set_size(btn, 60, 40);
    lv_obj_set_pos(btn, x, y);
    
    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, text);
    lv_obj_center(label);
    
    return btn;
}




void setup_music_controls(lv_obj_t *music_screen, NewType *data) {
    // 创建控制按钮容器
    lv_obj_t *control_cont = lv_obj_create(music_screen);
    lv_obj_set_size(control_cont, 600, 80);
    lv_obj_align(control_cont, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_flex_flow(control_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(control_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(control_cont, LV_OBJ_FLAG_SCROLLABLE);

    // 设置控制栏样式
    static lv_style_t cont_style;
    lv_style_init(&cont_style);
    lv_style_set_bg_color(&cont_style, lv_color_hex(0xF0F0F0));
    lv_style_set_radius(&cont_style, 10);
    lv_obj_add_style(control_cont, &cont_style, 0);

    // 上一曲按钮
    lv_obj_t *prev_btn = lv_btn_create(control_cont);
    lv_obj_set_size(prev_btn, 60, 60);
    lv_obj_t *prev_label = lv_label_create(prev_btn);
    lv_label_set_text(prev_label, LV_SYMBOL_PREV);
    lv_obj_center(prev_label);
    lv_obj_add_event_cb(prev_btn, prev_music_cb, LV_EVENT_CLICKED, data);

    // 播放按钮
    lv_obj_t *play_btn = lv_btn_create(control_cont);
    lv_obj_set_size(play_btn, 60, 60);
    lv_obj_t *play_label = lv_label_create(play_btn);
    lv_label_set_text(play_label, LV_SYMBOL_PLAY);
    lv_obj_center(play_label);
    lv_obj_add_event_cb(play_btn, Start_music_cb, LV_EVENT_CLICKED, data);

    // 暂停按钮
    lv_obj_t *pause_btn = lv_btn_create(control_cont);
    lv_obj_set_size(pause_btn, 60, 60);
    lv_obj_t *pause_label = lv_label_create(pause_btn);
    lv_label_set_text(pause_label, LV_SYMBOL_PAUSE);
    lv_obj_center(pause_label);
    lv_obj_add_event_cb(pause_btn, stop_music_cb, LV_EVENT_CLICKED, data);

    // 下一曲按钮
    lv_obj_t *next_btn = lv_btn_create(control_cont);
    lv_obj_set_size(next_btn, 60, 60);
    lv_obj_t *next_label = lv_label_create(next_btn);
    lv_label_set_text(next_label, LV_SYMBOL_NEXT);
    lv_obj_center(next_label);
    lv_obj_add_event_cb(next_btn, next_music_cb, LV_EVENT_CLICKED, data);
}


void setup_video_controls(lv_obj_t * video_screen, NewType * data) {
    // 创建控制条容器，确保在最上层且位于底部
    lv_obj_t *ctrl_bar = lv_obj_create(video_screen);

    // 使用flex布局自动排列按钮
    lv_obj_set_flex_flow(ctrl_bar, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(ctrl_bar, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    // 创建按钮（不再需要指定位置）
    lv_obj_t *btn_play = Create_VideoButton(ctrl_bar, 0, 0, "start");
    lv_obj_add_event_cb(btn_play, play_video_cb, LV_EVENT_CLICKED, data);
    
    lv_obj_t *btn_pause = Create_VideoButton(ctrl_bar, 0, 0, "pause");
    lv_obj_add_event_cb(btn_pause, pause_video_cb, LV_EVENT_CLICKED, data);
    
    lv_obj_t *btn_stop = Create_VideoButton(ctrl_bar, 0, 0, "stop");
    lv_obj_add_event_cb(btn_stop, stop_video_cb, LV_EVENT_CLICKED, data);
    
    lv_obj_t *btn_prev = Create_VideoButton(ctrl_bar, 0, 0, "prev");
    lv_obj_add_event_cb(btn_prev, prev_video_cb, LV_EVENT_CLICKED, data);
    
    lv_obj_t *btn_next = Create_VideoButton(ctrl_bar, 0, 0, "next");
    lv_obj_add_event_cb(btn_next, next_video_cb, LV_EVENT_CLICKED, data);

    lv_obj_t *btn_volumeadd = Create_VideoButton(ctrl_bar, 0, 0, "vol++");
    lv_obj_add_event_cb(btn_volumeadd, volumeadd_video_cb, LV_EVENT_CLICKED, data);

    lv_obj_t *btn_volumere = Create_VideoButton(ctrl_bar, 0, 0, "vol--");
    lv_obj_add_event_cb(btn_volumere, volumere_video_cb, LV_EVENT_CLICKED, data);

    lv_obj_t *btn_seekf = Create_VideoButton(ctrl_bar, 0, 0, "seek>>");
    lv_obj_add_event_cb(btn_seekf, seekf_video_cb, LV_EVENT_CLICKED, data);

    lv_obj_t *btn_seeks = Create_VideoButton(ctrl_bar, 0, 0, "seek<<");
    lv_obj_add_event_cb(btn_seeks, seeks_video_cb, LV_EVENT_CLICKED, data);
}
