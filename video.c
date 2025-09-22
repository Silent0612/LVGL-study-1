#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../lvgl/lvgl.h"
#include "../lvgl/demos/lv_demos.h"
#include "musicpic.h"
#include "DoubleCircularLinkedList.h"

// 播放视频
void play_video_cb(lv_event_t * e) {
    NewType * data = (NewType *)lv_event_get_user_data(e);
    BiNode ** v = data->v;
    
    // 如果已有视频在播放，先停止
    if (data->video_playing) {
        write(data->video_fd, "quit\n", 5);
        waitpid(data->video_pid, NULL, 0);
        close(data->video_fd);
    }
    
    // 创建命名管道
    mkfifo("/home/gzj/video_fifo", 0666);
    
    // 创建子进程播放视频
    pid_t pid = fork();
    if (pid == 0) { // 子进程
        char cmd[512];
        sprintf(cmd, "mplayer -slave -quiet -input file=/home/gzj/video_fifo -geometry 100:0 -zoom -x 700 -y 400 %s",
                (*v)->data);
        system(cmd);
        exit(0);
    } else { // 父进程
        data->video_pid = pid;
        data->video_fd = open("/home/gzj/video_fifo", O_WRONLY);
        data->video_playing = 1;
    }
}

// 停止视频
void stop_video_cb(lv_event_t * e) {
    NewType * data = (NewType *)lv_event_get_user_data(e);
    if (data->video_playing) {
        write(data->video_fd, "quit\n", 5);
        waitpid(data->video_pid, NULL, 0);
        close(data->video_fd);
        system("pkill -9 mplayer");
        unlink("/home/gzj/video_fifo");
        data->video_playing = 0;
    }
}

// 暂停/继续视频
void pause_video_cb(lv_event_t * e) {
    NewType * data = (NewType *)lv_event_get_user_data(e);
    if (data->video_playing) {
        write(data->video_fd, "pause\n", 6);
    }
}

// 下一个视频
void next_video_cb(lv_event_t * e) {
    NewType * data = (NewType *)lv_event_get_user_data(e);
    stop_video_cb(e); // 停止当前
    
    BiNode ** v = data->v;
    *v = (*v)->next; // 移动到下一个
    
    play_video_cb(e); // 播放新视频
}

// 上一个视频
void prev_video_cb(lv_event_t * e) {
    NewType * data = (NewType *)lv_event_get_user_data(e);
    stop_video_cb(e); // 停止当前
    
    BiNode ** v = data->v;
    *v = (*v)->prev; // 移动到上一个
    
    play_video_cb(e); // 播放新视频
}

// 声音加
void volumeadd_video_cb(lv_event_t * e) {
    NewType * data = (NewType *)lv_event_get_user_data(e);
    if (data->video_playing) {
        write(data->video_fd,"volume +5\n",strlen("volume +5\n"));
    }
}
//声音减
void volumere_video_cb(lv_event_t * e) {
    NewType * data = (NewType *)lv_event_get_user_data(e);
    if (data->video_playing) {
        write(data->video_fd,"volume -5\n",strlen("volume -5\n"));
    }
}
// 快进
void seekf_video_cb(lv_event_t * e) {
    NewType * data = (NewType *)lv_event_get_user_data(e);
    if (data->video_playing) {
        write(data->video_fd,"seek +5\n",strlen("seek +5\n"));
    }
}
//快退
void seeks_video_cb(lv_event_t * e) {
    NewType * data = (NewType *)lv_event_get_user_data(e);
    if (data->video_playing) {
        write(data->video_fd,"seek -5\n",strlen("seek -5\n"));
    }
}