/*
 * @Author: your name
 * @Date: 2020-03-05 10:23:42
 * @LastEditTime: 2020-03-10 11:56:10
 * @LastEditors: DahlMill
 * @Description: In User Settings Edit
 * @FilePath: /VSLAM/incc/ConvertCP.h
 */
#ifndef CONVERT_CP_H
#define CONVERT_CP_H
#endif

#include <sys/time.h>

struct POS
{
    int x;
    int y;
    int z;
    int yaw;
    int imgCount;
    unsigned char status;
    unsigned char status_;
    timeval tv;
};

struct CP_DATA
{
    POS SLAM;
    POS Chassis;
};

void ShowPosPC(CP_DATA get);
void ShowPosChassis(POS pos);
void ShowHexArr(unsigned char arr[], int len);

void PosData2ArrPC(CP_DATA &post, unsigned char *arr);
bool Arr2PosDataPC(CP_DATA &get, unsigned char *arr);

void Pos2ArrChassis(POS &pos, unsigned char *arr);
bool Arr2PosChassis(POS &pos, unsigned char *arr);