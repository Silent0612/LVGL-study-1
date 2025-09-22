#ifndef __FTP_CLIENT_H__
#define __FTP_CLIENT_H__

#include "lvgl/lvgl.h"
#define FTP_ROOT_DIR "/home/china/ftp"
#define FTP_LOCAL_DIR "/home/gzj"

//命令号 “整数值”
enum CMD_NO
{
    FTP_CMD_LIST,
    FTP_CMD_GET,
    FTP_CMD_PUT,
    FTP_CMD_BYE
};


void ftp_client_init(lv_obj_t *parent);
void ftp_client_close();
void send_put(int sock, unsigned char *arg1_data);
int Change_Data(unsigned char * data, int len);

#endif