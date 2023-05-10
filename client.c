#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

#define MAXLINE 1024

typedef struct GYROSCOPE_INFO_STRU_ {
    float gyro_x;
    float gyro_y;
    float gyro_z;
    float timestamp;
} GYROSCOPE_INFO_STRU;

typedef struct GSENSOR_INFO_STRU_ {
	float acc_x;
	float acc_y;
	float acc_z;
	float temp;
    float timestamp;
	/*temperature*/
} GSENSOR_INFO_STRU;

typedef enum {
    GPS_ON_POSITIONING,
    GPS_IS_FIXED,
    GPS_HARDWARE_ERR,
    GPS_POWER_CLOSED
} gps_locate_status_e_type;

typedef struct FW_GPS_INFO_STRU {
    int azimuth; /*0-359*/
    int SNR; /*(C/No) 00 -- 99 dB*/
    float accuracy; /*0|1-50 m*/
    float speed; /*0.0-999.9 km/h */
    float altitude; /*xxxxx.xm*/
    double longitude; /*xxx.xxxxxx*/
    double latitude; /*xx.xxxxxx*/
    char utc_time[32]; /*yyyymmddhhmmss*/
    float horizontal_accuracy; /*0-50 m*/
    float vertical_accuracy; /*0-50 m*/
    int satellites_num ; /*0-24*/
    gps_locate_status_e_type status;/*gps fixed/health status*/
} FW_GPS_INFO_STRU;

typedef struct GPS_INFO_PACKED {
    int azimuth; /*0-359*/
    int SNR; /*(C/No) 00 -- 99 dB*/
    float accuracy; /*0|1-50 m*/
    float speed; /*0.0-999.9 km/h */
    float altitude; /*xxxxx.xm*/
    double longitude; /*xxx.xxxxxx*/
    double latitude; /*xx.xxxxxx*/
    char utc_time[32]; /*yyyymmddhhmmss*/
    float horizontal_accuracy; /*0-50 m*/
    float vertical_accuracy; /*0-50 m*/
    int satellites_num ; /*0-24*/
    gps_locate_status_e_type status;/*gps fixed/health status*/
} __attribute__((packed)) GPS_INFO_PACKED;


typedef struct SOCKET_MESSAGE {
    char data[512];
    int length;
} __attribute__((packed)) SOCKET_MESSAGE;

static GYROSCOPE_INFO_STRU gyro_data = {1.0, 2.0, 3.0, 123.0};
static GSENSOR_INFO_STRU gsensor_data = {0, 0, 1, 0, 123.0};
static FW_GPS_INFO_STRU gps_data = {0, 0, 0, 0, 1, 2, 3, "20230510122800", 0, 0, 0, GPS_IS_FIXED };
static GPS_INFO_PACKED gps_data_packed;
static SOCKET_MESSAGE socket_message;

int main(int argc, char** argv) {
    // char address[] = "172.23.168.149";
    // char address[] = "192.168.158.100";
    char address[] = "127.0.0.1";
    int    sockfd, n;
    char    recvline[MAXLINE], sendline[MAXLINE];
    struct sockaddr_in    servaddr;

    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
        exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(12345);
    if( inet_pton(AF_INET, address, &servaddr.sin_addr) <= 0){
        printf("inet_pton error for %s\n", address);
        exit(0);
    }

    if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }


    printf("gyro_data size: %d\n", sizeof(gyro_data));
    printf("gsensor_data size: %d\n", sizeof(gsensor_data));
    printf("gps type size: %d\n", sizeof(FW_GPS_INFO_STRU));
    printf("gps dat size: %d\n", sizeof(gps_data));
    printf("gps dat size: %d\n", sizeof(GPS_INFO_PACKED));

    int i = 0;
    while(1) {
        gps_data.azimuth = i;
        int buf_len = send_gps_data();
        printf("gps send len: %d %d\n", buf_len, i);
        if( send(sockfd, socket_message.data, buf_len, 0) < 0) {
            printf("send gyro msg error: %s(errno: %d)\n", strerror(errno), errno);
            exit(0);
        }
        gsensor_data.acc_x = i;
        buf_len = send_gsensor_data();
        if( send(sockfd, socket_message.data, buf_len, 0) < 0) {
            printf("send gyro msg error: %s(errno: %d)\n", strerror(errno), errno);
            exit(0);
        }
        gyro_data.gyro_x = i;
        buf_len = send_gyroscope_data();
        if( send(sockfd, socket_message.data, buf_len, 0) < 0) {
            printf("send gyro msg error: %s(errno: %d)\n", strerror(errno), errno);
            exit(0);
        }
        i++;
        usleep(1000*10);
    }

    memset(recvline, 0, MAXLINE);
    if (recv(sockfd, recvline, MAXLINE, 0) < 0) {
        printf("recv msg error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    } else {
        printf("recv msg: %s\n", recvline);
    }

    close(sockfd);
    exit(0);
}

int send_gyroscope_data() {
    int type = 0;
    char *p_type = socket_message.data;
    char *p_data = p_type + sizeof(int);
    char *p_end = p_data + sizeof(gyro_data);
    memset(socket_message.data, 0, sizeof(256));
    memcpy(p_type, &type, sizeof(int));
    memcpy(socket_message.data + sizeof(int), &gyro_data, sizeof(gyro_data));
    *p_end = '\n';
    return sizeof(gyro_data) + 5;
}

int send_gsensor_data() {
    int type = 1;
    char *p_type = socket_message.data;
    char *p_data = p_type + sizeof(int);
    char *p_end = p_data + sizeof(gsensor_data);
    memset(socket_message.data, 0, sizeof(256));
    memcpy(p_type, &type, sizeof(int));
    memcpy(socket_message.data + sizeof(int), &gsensor_data, sizeof(gsensor_data));
    *p_end = '\n';
    return sizeof(gsensor_data) + 5;
}

int send_gps_data() {
    gps_data_packed.azimuth = gps_data.azimuth;
    gps_data_packed.SNR = gps_data.SNR;
    gps_data_packed.accuracy = gps_data.accuracy; /*0|1-50 m*/
    gps_data_packed.speed = gps_data.speed; /*0.0-999.9 km/h */
    gps_data_packed.altitude = gps_data.altitude; /*xxxxx.xm*/
    gps_data_packed.longitude = gps_data.longitude; /*xxx.xxxxxx*/
    gps_data_packed.latitude = gps_data.latitude; /*xx.xxxxxx*/
    memcpy(gps_data_packed.utc_time, gps_data.utc_time, sizeof(gps_data.utc_time));
    gps_data_packed.horizontal_accuracy = gps_data.horizontal_accuracy; /*0-50 m*/
    gps_data_packed.vertical_accuracy = gps_data.vertical_accuracy; /*0-50 m*/
    gps_data_packed.satellites_num =  gps_data.satellites_num; /*0-24*/
    gps_data_packed.status = gps_data.status;
    int type = 2;
    char *p_type = socket_message.data;
    char *p_data = p_type + sizeof(int);
    char *p_end = p_data + sizeof(gps_data_packed);
    memset(socket_message.data, 0, sizeof(256));
    memcpy(p_type, &type, sizeof(int));
    memcpy(socket_message.data + sizeof(int), &gps_data_packed, sizeof(gps_data_packed));
    *p_end = '\n';
    return sizeof(gps_data_packed) + 5;
}