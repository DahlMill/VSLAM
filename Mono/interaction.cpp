/*
 * @Author: DahlMill
 * @Date: 2020-03-04 10:59:15
 * @LastEditTime: 2020-03-11 16:35:40
 * @LastEditors: DahlMill
 * @Description: 
 * @FilePath: /VSLAM/Mono/Interaction.cpp
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>

#include <sys/socket.h>
#include <netinet/in.h>
#include "Semaphore.h"
#include "ConvertCP.h"
#include <arpa/inet.h>
#include <app_uart.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ShmCom.h"
#include <stdio.h>
#include <errno.h>
#include <log.h>

using namespace std;

#define SEM_SEED 1773
#define SHM_SEED 1774

#define SEM_SEED_Chassis 1333
#define SHM_SEED_Chassis 1334

#define TCP_TEST
#define APP_UART "/dev/ttyS2"
#define SOCKET_FILE "WifiConfig"
#define DEVICE_LED_UART_BAUD_RATE 115200

static int livUartFd;
static int livSocketFd;

int iSemID; // Semaphore id
int iShmID; // shared-memory id
// int iEndFlag = 0;
void *pSharedMemory = (void *)0;
SHM_POS *pSharedStuff;

int iSemID2Chassis; // Semaphore id
int iShmID2Chassis; // shared-memory id
// int iEndFlag = 0;
void *pSharedMemory2Chassis = (void *)0;
SHM_POS *pSharedStuff2Chassis;

POS stuPosSLAM;
POS stuPosChassis;

void InitTCP(void);
void InitUart(void);
void InitShm(void);
void InitShmChassis(void);

void TestUart(void)
{
    unsigned char buf[512];
    unsigned char data_str[4] = {'a', 'b', 'c', 'd'};
    int i;

    log_info(">====Welcome to Uart Test=====<");
    cout << "Uart :" << APP_UART << endl;

    livUartFd = app_uart_open(livUartFd, APP_UART);
    app_uart_Init(livUartFd, DEVICE_LED_UART_BAUD_RATE, 0, 8, 1, 'N');

    while (1)
    {
        app_uart_write(livUartFd, data_str, 4);
        usleep(100 * 1000);
        app_uart_read(livUartFd, buf, sizeof(buf), 2000);
        for (i = 0; i < 16; i++)
            printf("data = %x \r\n", buf[i]);
    }
}

void TestTCP(void)
{
    InitTCP();

    while (1)
    {
        char writeData[] = {'h', 'e', 'l', 'l', 'o', '\n'};
        int writelen = sizeof(writeData) / sizeof(unsigned char);
        write(livSocketFd, writeData, writelen);
        usleep(1000 * 1000);
    }
}

/**
 * @description: TCP数据上报线程
 * @param {std::mutex} SLAM锁 底盘锁
 * @return: 
 */
void ThrTestTCP(void)
{
    cout << "ThrTestTCP Create OK" << endl;

    InitTCP();
    CP_DATA post;
    unsigned char writeData[24] = {0};
    do
    {
        // cout << "ThrTestTCP RUN" << endl;
        memset(&post, 0, sizeof(CP_DATA));

        // cout << "ThrTestTCP Lock" << endl;

        memcpy(&(post.SLAM), &stuPosSLAM, sizeof(POS));
        memcpy(&(post.Chassis), &stuPosChassis, sizeof(POS));
        
        PosData2ArrPC(post, writeData);
        // write(livUartFd, writeData, 24);
        write(livSocketFd, writeData, 24);
        
        usleep(30 * 1000);
    }while (1);
}

/**
 * @description: 读取共享内存中的 SLAM 数据
 * @param {std::mutex} SLAM锁
 * @return: 
 */
void ThrSemSLAM(void)
{
    cout << "ThrSemSLAM Create OK" << endl;
    InitShm();

    ofstream os;                                   //创建一个文件输出流对象
    os.open("imglog.txt", ios::out | ios::trunc);  //将对象与文件关联

    do
    {
        // cout << "ThrSemSLAM RUN" << endl;
        SemaphoreP(iSemID);
        
        // cout << "ThrSemSLAM Lock" << endl;
        memcpy(&stuPosSLAM, (char *)pSharedStuff->shmSp, sizeof(POS));
        // cout << "ThrSemSLAM " << stuPosSLAM.x << " " << stuPosSLAM.y << " " << stuPosSLAM.z << endl;
        
        // iEndFlag = pSharedStuff->endFlag;
        SemaphoreV(iSemID);

        if(stuPosSLAM.imgCount > 0)
        {
            timeval tv;
            gettimeofday(&tv, NULL);
            ostringstream strLog;
            strLog << stuPosSLAM.imgCount << " " << tv.tv_sec << " " << tv.tv_usec << " " << stuPosChassis.x << " " << stuPosChassis.y << " " << stuPosChassis.yaw << '\n';
            os << strLog.str(); //将输入的内容放入txt文件中
            os.flush();
        }

        usleep(30 * 1000);
    }while (1);

    os.close();
    
    if (shmdt(pSharedMemory) == -1)
    {
        fprintf(stderr, "shmdt failed.\n");
    }
}

/**
 * @description: 向共享内存中写入底盘数据
 * @param {std::mutex} SLAM锁
 * @return: 
 */
void ThrSemChassis(void)
{
    cout << "ThrSemChassis Create OK" << endl;
    InitShmChassis();

    do
    {
        SemaphoreP(iSemID2Chassis);
        /* Write it into shared memory */
        // cout << "memcpy" << endl;
        memcpy((char *)pSharedStuff2Chassis->shmSp, &stuPosChassis, sizeof(POS));
        // pSharedStuff2Chassis->endFlag = 0;
        // cout << "SemaphoreV" << endl;
        SemaphoreV(iSemID2Chassis);
        usleep(30 * 1000);
    }while (1);
    

    if (shmdt(pSharedMemory2Chassis) == -1)
    {
        fprintf(stderr, "shmdt failed.\n");
    }
}

void ThrUartChassis(void)
{
    cout << "ThrUartChassis Create OK" << endl;
    InitUart();

    unsigned char readData[64];
    unsigned char writeData[18];

    do
    {
        memset(readData, 0, sizeof(readData));
        app_uart_read(livUartFd, readData, sizeof(readData), 30);

        // cout << "len is " << endl;
        // for(int i = 0; i<len; i ++)
        //     cout << hex << (int)readData[i] << " ";
        // cout << dec <<endl;

        Arr2PosChassis(stuPosChassis, readData);

        // ShowPosChassis(stuPosChassis);

        Pos2ArrChassis(stuPosSLAM, writeData);

        app_uart_write(livUartFd, writeData, 18);

        usleep(30 * 1000);

    } while (1);

    
}

int main(int argc, char *argv[])
{
    memset(&stuPosSLAM, 0, sizeof(POS));
    memset(&stuPosChassis, 0, sizeof(POS));
    
#ifdef TCP_TEST
    std::thread thrTestTCP = std::thread(ThrTestTCP);
#endif
    std::thread thrSemSLAM = std::thread(ThrSemSLAM);
    std::thread thrSemChassis = std::thread(ThrSemChassis);
    std::thread thrUartChassis = std::thread(ThrUartChassis);

    do
    {
        
        usleep(1000 * 1000);

    } while (1);

    /* Over */
    printf("\nInteraction over!\n");

    exit(EXIT_SUCCESS);
}

void InitShm(void)
{
    /* Create Semaphore */
    cout << "InitShm" << endl;
    iSemID = semget((key_t)SEM_SEED, 1, 0666 | IPC_CREAT);
    if (iSemID == -1)
    {
        fprintf(stderr, "semget failed.\n");
        exit(EXIT_FAILURE);
    }

    /* Init shared-memory */
    iShmID = shmget((key_t)SHM_SEED, sizeof(SHM_POS), 0666 | IPC_CREAT);
    if (iShmID == -1)
    {
        fprintf(stderr, "shmget failed.\n");
        exit(EXIT_FAILURE);
    }

    pSharedMemory = shmat(iShmID, (void *)0, 0);
    if (pSharedMemory == (void *)-1)
    {
        fprintf(stderr, "shmat failed.\n");
        exit(EXIT_FAILURE);
    }
    pSharedStuff = (SHM_POS *)pSharedMemory;

    // printf("FRAME_CNT: %d\n", frame_cnt);
    /* 
    * 必须先置0，
    * 否则会因生产者进程的异常退出未释放信号量而导致程序出错 
    */
    SetSemValue(iSemID, 0);
}

void InitShmChassis(void)
{
    cout << "InitShmChassis" << endl;
    /* Create Semaphore */
    iSemID2Chassis = semget((key_t)SEM_SEED_Chassis, 1, 0666 | IPC_CREAT);
    if (iSemID2Chassis == -1)
    {
        fprintf(stderr, "semget failed.\n");
        exit(EXIT_FAILURE);
    }

    /* Init shared-memory */
    iShmID2Chassis = shmget((key_t)SHM_SEED_Chassis, sizeof(SHM_POS), 0666 | IPC_CREAT);
    if (iShmID2Chassis == -1)
    {
        fprintf(stderr, "shmget failed.\n");
        exit(EXIT_FAILURE);
    }

    pSharedMemory2Chassis = shmat(iShmID2Chassis, (void *)0, 0);
    if (pSharedMemory2Chassis == (void *)-1)
    {
        fprintf(stderr, "shmat failed.\n");
        exit(EXIT_FAILURE);
    }
    pSharedStuff2Chassis = (SHM_POS *)pSharedMemory2Chassis;

    // printf("FRAME_CNT: %d\n", frame_cnt);
    /* 
    * 必须先置0，
    * 否则会因生产者进程的异常退出未释放信号量而导致程序出错 
    */
    SetSemValue(iSemID2Chassis, 0);
}

void InitUart(void)
{
    // 串口初始化
    livUartFd = app_uart_open(livUartFd, APP_UART);
    app_uart_Init(livUartFd, DEVICE_LED_UART_BAUD_RATE, 0, 8, 1, 'N');
}

void InitTCP(void)
{
    ifstream socketFile(SOCKET_FILE);
    string strIP;
    string strPort;
    if (!socketFile.is_open())
    {
        cout << SOCKET_FILE << " open error" << endl;
    }

    if (!getline(socketFile, strIP))
    {
        cout << SOCKET_FILE << " read error" << endl;
    }
    cout << "strIP " << strIP << endl;

    if (!getline(socketFile, strPort))
    {
        cout << SOCKET_FILE << "read error" << endl;
    }
    cout << "strPort " << strPort << endl;

    socketFile.close();

    // int
    livSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (livSocketFd == -1)
    {
        cout << "socket create worry：" << endl;
        exit(-1);
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;

    addr.sin_port = htons(atoi(strPort.data()));
    addr.sin_addr.s_addr = inet_addr(strIP.data());

    int res = connect(livSocketFd, (struct sockaddr *)&addr, sizeof(addr));
    if (res == -1)
    {
        cout << "bind link worry：" << endl;
        exit(-1);
    }
    cout << "bind link ok" << endl;
}
