#include "network_time.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/socket.h>
#include <errno.h>
#include "musicpic.h"

#define NTP_SERVER "ntp.aliyun.com"  // 主NTP服务器
#define BACKUP_NTP_SERVER "182.92.12.11" // 备用NTP服务器IP
#define NTP_PORT 123
#define NTP_TO_UNIX_DELTA 2208988800UL
#define NTP_PACKET_SIZE 48
#define TIMEOUT_SEC 5

static lv_timer_t *update_timer = NULL;
static lv_obj_t *time_label = NULL;

// 从指定服务器获取NTP时间
static time_t get_ntp_time_from_server(const char* server) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    // 设置超时
    struct timeval timeout = {TIMEOUT_SEC, 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(NTP_PORT)
    };
    
    // 将服务器地址转换为网络格式
    if (inet_pton(AF_INET, server, &server_addr.sin_addr) <= 0) {
        close(sock);
        return -1;
    }

    // 构建NTP请求
    uint8_t ntp_packet[NTP_PACKET_SIZE] = {0x1B};

    if (sendto(sock, ntp_packet, sizeof(ntp_packet), 0, 
              (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        close(sock);
        return -1;
    }

    struct sockaddr_in from;
    socklen_t from_len = sizeof(from);
    ssize_t recv_len = recvfrom(sock, ntp_packet, sizeof(ntp_packet), 0, 
                              (struct sockaddr *)&from, &from_len);
    close(sock);

    if (recv_len < 0) return -1;

    // 提取时间并应用时区补偿(+8小时)
    uint32_t ntp_time = ntohl(*(uint32_t *)(ntp_packet + 40));
    return (time_t)(ntp_time - NTP_TO_UNIX_DELTA + 8*3600);
}

// 获取NTP时间（尝试主服务器和备用服务器）
static time_t get_ntp_time() {
    time_t ntp_time = get_ntp_time_from_server(NTP_SERVER);
    if (ntp_time > 0) {
        return ntp_time;
    }
    
    // 主服务器失败，尝试备用服务器
    return get_ntp_time_from_server(BACKUP_NTP_SERVER);
}

// 更新显示
static void ntp_update_cb(lv_timer_t *timer) {
    time_t ntp_time = get_ntp_time();
    if (ntp_time > 0) {
        struct tm *timeinfo = localtime(&ntp_time);
        char time_str[32];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);
        lv_label_set_text(time_label, time_str);
    } else {
        lv_label_set_text(time_label, "Sync failed");
    }
}

void network_time_init(lv_obj_t *parent) {
    // 创建容器
    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    
    // 返回按钮
    lv_obj_t *back_btn = lv_btn_create(cont);
    lv_obj_set_size(back_btn, 60, 40);
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, LV_SYMBOL_LEFT);
    lv_obj_center(back_label);
    lv_obj_add_event_cb(back_btn, back_btn_cb, LV_EVENT_CLICKED, NULL);
    
    // 时间显示
    time_label = lv_label_create(cont);
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_24, 0);
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(time_label, "get time now...");
    
    // 创建定时器
    if (update_timer) lv_timer_del(update_timer);
    update_timer = lv_timer_create(ntp_update_cb, 30000, NULL);
    ntp_update_cb(NULL);  // 立即执行
}

void network_time_close() {
    if (update_timer) {
        lv_timer_del(update_timer);
        update_timer = NULL;
    }
}
