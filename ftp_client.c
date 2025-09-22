#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "lvgl/lvgl.h"
#include "lvgl/src/core/lv_obj.h"
#include "lvgl/src/widgets/textarea/lv_textarea.h"
#include "ftp_client.h"
#include "musicpic.h"
#include <poll.h>

// 全局变量存储UI组件和连接状态
static lv_obj_t *ip_input, *port_input, *file_input, *log_area;
static int sock_fd = -1; // FTP服务器连接套接字

static void connect_btn_cb(lv_event_t *e);
static void put_btn_cb(lv_event_t *e);
static void list_btn_cb(lv_event_t *e);

static void ftp_stdin_cb(lv_timer_t *timer) {
    struct pollfd fds = {.fd = STDIN_FILENO, .events = POLLIN};
    if (poll(&fds, 1, 0) > 0) {
        char cmd[256];
        if (fgets(cmd, sizeof(cmd), stdin)) {
            cmd[strcspn(cmd, "\n")] = 0; // Remove newline
            
            if (strcmp(cmd, "connect") == 0) {
                connect_btn_cb(NULL);
            } else if (strncmp(cmd, "put ", 4) == 0) {
                const char *filename = cmd + 4;
                lv_textarea_set_text(file_input, filename);
                put_btn_cb(NULL);
            } else if (strcmp(cmd, "list") == 0) {
                list_btn_cb(NULL);
            } else {
                lv_textarea_add_text(log_area, "Unknown command\n");
            }
        }
    }
}



// 连接FTP服务器回调函数
static void connect_btn_cb(lv_event_t *e) {
    if (sock_fd != -1) {
        lv_textarea_add_text(log_area, "已连接服务器\n");
        return;
    }

    // 获取输入的IP和端口
    const char *ip = "192.168.137.205";
    const char *port_str = "55555";
    int port = atoi(port_str);

    // 创建TCP连接（参考server的Create_Socket逻辑）
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in ser;
    memset(&ser, 0, sizeof(ser));
    ser.sin_family = AF_INET;
    ser.sin_port = htons(port);
    ser.sin_addr.s_addr = inet_addr(ip);

    if (connect(sock_fd, (struct sockaddr *)&ser, sizeof(ser)) == -1) {
        lv_textarea_add_text(log_area, "Connection failed\n");
        sock_fd = -1;
        return;
    }

    lv_textarea_add_text(log_area, "Connection success\n");
}

// 发送PUT命令（上传文件）回调
static void put_btn_cb(lv_event_t *e) {
    if (sock_fd == -1) {
        lv_textarea_add_text(log_area, "please connection ser\n");
        return;
    }

    const char *filename = lv_textarea_get_text(file_input);
    send_put(sock_fd, (unsigned char *)filename); // 调用现有发送逻辑
    lv_textarea_add_text(log_area, "send put\n");
}

// 发送LIST命令（列出文件）回调
static void list_btn_cb(lv_event_t *e) {
    if (sock_fd == -1) {
        lv_textarea_add_text(log_area, "please connection ser\n");
        return;
    }

    // 构造LIST命令包（参考put的包结构）
    unsigned char send_buf[1024] = {0xc0};
    int i = 1;
    enum CMD_NO cmd_no = FTP_CMD_LIST;
    int pkg_len = 11 + 4 + 0; // arg1_len为0（LIST无需参数）

    // 填充包头（pkg_len、cmd_no、arg1_len）
    send_buf[i++] = (pkg_len >> 0) & 0xff;
    send_buf[i++] = (pkg_len >> 8) & 0xff;
    send_buf[i++] = (pkg_len >> 16) & 0xff;
    send_buf[i++] = (pkg_len >> 24) & 0xff;

    send_buf[i++] = (cmd_no >> 0) & 0xff;
    send_buf[i++] = (cmd_no >> 8) & 0xff;
    send_buf[i++] = (cmd_no >> 16) & 0xff;
    send_buf[i++] = (cmd_no >> 24) & 0xff;

    send_buf[i++] = 0; // arg1_len为0
    send_buf[i++] = 0;
    send_buf[i++] = 0;
    send_buf[i++] = 0;

    // 计算校验和
    unsigned char check_num = 0;
    for (int j = 1; j < i; j++) check_num += send_buf[j];
    send_buf[i++] = check_num;
    send_buf[i++] = 0xc0;

    // 转换特殊字符并发送
    int len = Change_Data(send_buf, i);
    send(sock_fd, send_buf, len, 0);
    lv_textarea_add_text(log_area, "send list\n");

    // 接收服务器响应（简化版，实际需循环读取）
    unsigned char resp[4096] = {0};
    recv(sock_fd, resp, sizeof(resp), 0);
    lv_textarea_add_text(log_area, "ser list:\n");
    lv_textarea_add_text(log_area, (char *)(resp + 14)); // 跳过协议头
}

// 初始化FTP客户端UI
void ftp_client_init(lv_obj_t *parent) {
    // 创建容器放置所有组件
    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_set_size(cont, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    static lv_style_t cont_style;
    lv_style_init(&cont_style);
    lv_style_set_pad_all(&cont_style, 10);
    lv_obj_add_style(cont, &cont_style, 0);

    // 标题
    lv_obj_t *title = lv_label_create(cont);
    lv_label_set_text(title, "FTP Client (192.168.137.205:55555)");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_set_width(title, lv_pct(100));
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);

    // 文件名输入框
    lv_obj_t *file_label = lv_label_create(cont);
    lv_label_set_text(file_label, "Filename:");
    file_input = lv_textarea_create(cont);
    lv_textarea_set_placeholder_text(file_input, "File name");
    lv_obj_set_width(file_input, lv_pct(90));
    lv_textarea_set_one_line(file_input, true);

    // 按钮组
    lv_obj_t *btn_cont = lv_obj_create(cont);
    lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_width(btn_cont, lv_pct(100));
    lv_obj_set_flex_align(btn_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *connect_btn = lv_btn_create(btn_cont);
    lv_obj_set_size(connect_btn, 120, 40);
    lv_obj_t *connect_label = lv_label_create(connect_btn);
    lv_label_set_text(connect_label, "Connect");
    lv_obj_center(connect_label);
    lv_obj_add_event_cb(connect_btn, connect_btn_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *put_btn = lv_btn_create(btn_cont);
    lv_obj_set_size(put_btn, 120, 40);
    lv_obj_t *put_label = lv_label_create(put_btn);
    lv_label_set_text(put_label, "Upload");
    lv_obj_center(put_label);
    lv_obj_add_event_cb(put_btn, put_btn_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *list_btn = lv_btn_create(btn_cont);
    lv_obj_set_size(list_btn, 120, 40);
    lv_obj_t *list_label = lv_label_create(list_btn);
    lv_label_set_text(list_label, "List Files");
    lv_obj_center(list_label);
    lv_obj_add_event_cb(list_btn, list_btn_cb, LV_EVENT_CLICKED, NULL);

    // 添加返回主界面按钮
    lv_obj_t *back_btn = lv_btn_create(cont);
    lv_obj_set_size(back_btn, 80, 40);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "Back");
    lv_obj_center(back_label);
    lv_obj_add_event_cb(back_btn, back_btn_cb, LV_EVENT_CLICKED, NULL);

    // 日志显示区域
    log_area = lv_textarea_create(cont);
    lv_textarea_set_accepted_chars(log_area, "");
    lv_textarea_set_max_length(log_area, 0);
    lv_obj_set_style_bg_opa(log_area, LV_OPA_TRANSP, LV_PART_CURSOR);
    lv_obj_set_height(log_area, 200);
    lv_obj_set_width(log_area, LV_HOR_RES - 40);

    // 添加stdin监听定时器
    lv_timer_create(ftp_stdin_cb, 100, NULL);
    
    // 添加使用说明
    lv_textarea_add_text(log_area, "Usage:\n");
    lv_textarea_add_text(log_area, " - Type 'connect' to connect\n");
    lv_textarea_add_text(log_area, " - Type 'put <filename>' to upload\n");
    lv_textarea_add_text(log_area, " - Type 'list' to list files\n");
}

// 关闭FTP连接
void ftp_client_close() {
    if (sock_fd != -1) {
        close(sock_fd);
        sock_fd = -1;
    }
}



void send_put(int sock , unsigned char * arg1_data)
{
    //1 4 4 4 ? 1 1
    //包头 pkg_len cmd_no arg1_len arg1_data 校验和 包尾

    unsigned char send_m[1024] = {0xc0};//保存要发送的数据
    int i = 1;
    int arg1_len = strlen(arg1_data);

    int pkg_len = 11 + 4 + arg1_len;
    enum CMD_NO cmd_no = FTP_CMD_PUT;


    send_m[i++] = (pkg_len >> 0) & 0xff;
    send_m[i++] = (pkg_len >> 8) & 0xff;
    send_m[i++] = (pkg_len >> 16) & 0xff;
    send_m[i++] = (pkg_len >> 24) & 0xff;

    send_m[i++] = (cmd_no >> 0) & 0xff;
    send_m[i++] = (cmd_no >> 8) & 0xff;
    send_m[i++] = (cmd_no >> 16) & 0xff;
    send_m[i++] = (cmd_no >> 24) & 0xff; 

    send_m[i++] = (arg1_len >> 0) & 0xff;
    send_m[i++] = (arg1_len >> 8) & 0xff;
    send_m[i++] = (arg1_len >> 16) & 0xff;
    send_m[i++] = (arg1_len >> 24) & 0xff;

    strncpy(send_m + i , arg1_data , arg1_len);
    i = i + arg1_len;

    unsigned char check_num = 0;
    for(int j = 1 ; j < i ; j++)
    {
        check_num += send_m[j];
    }
    send_m[i++] = check_num;
    send_m[i++] = 0xc0;
 
    int len = Change_Data(send_m , i);
   
    int ret = send(sock , send_m , len , 0);
    if(ret == -1)
    {
        perror("send-put error");
		close(sock);
		return ;
    }

    sleep(2);

    unsigned char resp[8192] = {0};
    unsigned char file[4096] = {0};

    //读取文件内容把文件内容放到file里面
    char path[256] = {0};
    sprintf(path , "%s/%s" , FTP_LOCAL_DIR , arg1_data);
    printf("打印客户端目录文件路径: %s\n" , path);

    int fd = open(path , O_RDWR);
    if(fd == -1)
    {
        perror("open error");
        return ;
    }
    while(1)
    {   
        char buf[512];
        int ret = read(fd , buf , 512);
        if(ret == -1)
        {
            perror("read error");
            return ;
        }
        else if(ret == 0)
        {
            break;
        }
        strncat(file , buf , ret);
    }
    

    char result = strlen(file) ? 1 : 0;
    int pkg_len_1;
    int file_len;
    if(result)
    {   
        file_len = strlen(file);
        pkg_len_1 = 15 + file_len + 1;
    }
    else
    {
        pkg_len_1 = 15 + 4 + 1;
        file_len = 4;
    }


    enum CMD_NO cmd_no_1 = FTP_CMD_PUT;

    int k = 0;
    resp[k++] = 0xc0; //包头
    //pkg_len 数据包长度
    resp[k++] = (pkg_len_1 >> 0) & 0xff;
    resp[k++] = (pkg_len_1 >> 8) & 0xff;
    resp[k++] = (pkg_len_1 >> 16) & 0xff;
    resp[k++] = (pkg_len_1 >> 24) & 0xff;
    //cmd_no 命令号
    resp[k++] = (cmd_no_1 >> 0) & 0xff;
    resp[k++] = (cmd_no_1 >> 8) & 0xff;
    resp[k++] = (cmd_no_1 >> 16) & 0xff;
    resp[k++] = (cmd_no_1 >> 24) & 0xff;
    //result 结果
    resp[k++] = result;
    //resp_len 真正内容的长度
    resp[k++] = (file_len >> 0) & 0xff;
    resp[k++] = (file_len >> 8) & 0xff;
    resp[k++] = (file_len >> 16) & 0xff;
    resp[k++] = (file_len >> 24) & 0xff;
    //resq_conent 回复的内容
    if(result)
    {
        strncpy(resp + k, file, file_len);
    }
    else
    {
        //将错误码 1 : FTP文件夹为空 (list failed)
        //保存到 resp + i 指向的四个字节处
        *((int *)(resp + k)) = 1;
    }
    k += file_len; 
    unsigned char check_sum = 0;
    for(int j = 1 ; j < k ; j++)
    {
        check_sum += resp[j];
    }
    resp[k++] = check_sum;
    resp[k++] = 0xc0;

    //将数据包中有效数据中出现 0xc0 或 0xdd的字节进行转换
	int len1 = Change_Data(resp , k);
    
    for(int n = 0 ; n < len1 ; n++)
    {
        printf("%02x ", resp[n]);
    }
    printf("\n");
    // printf("%d\n" , pkg_len_1);

	//将resp保存的数据包，发送至客户端 
	int ret1 = send(sock, resp , len1, 0);
	if(ret1 == -1)
	{
		perror("send put error");
		return ;
	}

}



/*
	Change_Data : 将数据包中有效数据中出现 0xc0 或 0xdd的字节进行转换
	@resp : 指向待转换的数据包内容 
	@len : 待转换的数据内容的字节数 
	返回值: 
		返回转换之后的数据包内容长度 
*/
int Change_Data(unsigned char * resp , int len)
{
    unsigned char * p = malloc(4096);
    int index;
    for(index = 0 ; index < len ; index++)
    {
        p[index] = resp[index];
    }
    int i = 0;
    int j = 0;
    resp[i++] = p[j++];

    while(j < len - 1)
    {   
        if(p[j] == 0xc0)
        {
            resp[i++] = 0xdd;
            resp[i++] = 0xdb;
        }
        else if(p[j] == 0xdd)
        {
            resp[i++] = 0xdd;
            resp[i++] = 0xdc;
        }
        else
        {
            resp[i++] = p[j];
        }
        j++;
    }
    resp[i++] = 0xc0;
    free(p);
    return i;
}



/*
	Check_Data : 验证数据包的可靠性
	@data : 指向待验证的数据包
	@len : 表示待验证的数据包里面数据的字节数
	返回值: 
		1 - 可靠 
		0 - 不可靠
*/
int Check_Data(unsigned char * data, int len)
{
    int pkg_len = (data[4] << 24) | (data[3] << 16) | (data[2] << 8) | data[1];

    if(pkg_len != len)
    {
        printf("message len error : pkg_len = %d, real_len = %d\n",pkg_len, len);
		return 0;
    }

    int i = 1;
    unsigned char check_num = 0;
    for(i = 1 ; i < len-2 ; i++)
    {
        check_num += data[i];
    }

    if(data[len-2] != check_num)
    {
        printf("message check error!\n");
		return 0;
	}
	return 1;
}