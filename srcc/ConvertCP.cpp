#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include "ConvertCP.h"

using namespace std;

/**
 * @description: 将一个int数据分解到两个字节中
 * @param {int} 需要分解的数据 {unsigend char} 需要存储的字节指针 
 * @return: 
 */
void CovInt2UChar(int data, unsigned char &hBit, unsigned char &lBit)
{
    hBit = data >> 8;
    lBit = data;
}

/**
 * @description: 两个字节的拼成一个有效数据
 * @param {unsigned char} 需要拼接的数据
 * @return: 拼接结果
 */
short CovUChar2Int(unsigned char hBit, unsigned char lBit)
{
    short data = (hBit << 8 | lBit);
    // data = (data < 32768) ? data : (data - 65536);
    return data;
}

/**
 * @description: 将位姿数据转换为数组 Chassis
 * @param {CP_DATA} SLAM 和底盘的位姿 {unsigned char} 需要存储的字节数组指针
 * @return: 
 */
void Pos2ArrChassis(POS &pos, unsigned char *arr)
{
    arr[0] = 0x55;
    arr[1] = 0x55;
    arr[2] = 0xAA;
    arr[3] = 0xAA;

    CovInt2UChar(pos.x, arr[4], arr[5]);
    CovInt2UChar(pos.y, arr[6], arr[7]);
    CovInt2UChar(pos.z, arr[8], arr[9]);
    CovInt2UChar(pos.yaw, arr[10], arr[11]);

    arr[12] = pos.status;
    arr[13] = pos.status_;

    int checkSum = 0;
    for(int i = 4; i < 13 + 1; i++)
    {
        checkSum += arr[i];
    }

    arr[14] = checkSum >> 8;
    arr[15] = checkSum;

    arr[16] = 0x55;
    arr[17] = 0xBB;

}

/**
 * @description: 将位姿数据转换为数组 PC
 * @param {CP_DATA} SLAM 和底盘的位姿 {unsigned char} 需要存储的字节数组指针
 * @return: 
 */
void PosData2ArrPC(CP_DATA &post, unsigned char *arr)
{   
    arr[0] = 0x55;
    arr[1] = 0x55;
    arr[2] = 0xAA;
    arr[3] = 0xAA;

    CovInt2UChar(post.SLAM.x, arr[4], arr[5]);
    CovInt2UChar(post.SLAM.y, arr[6], arr[7]);
    CovInt2UChar(post.SLAM.z, arr[8], arr[9]);
    CovInt2UChar(post.SLAM.yaw, arr[10], arr[11]);

    CovInt2UChar(post.Chassis.x, arr[12], arr[13]);
    CovInt2UChar(post.Chassis.y, arr[14], arr[15]);
    CovInt2UChar(post.Chassis.yaw, arr[16], arr[17]);

    arr[18] = post.SLAM.status;
    arr[19] = post.Chassis.status;

    int checkSum = 0;
    for(int i = 4; i < 19 + 1; i++)
    {
        checkSum += arr[i];
    }

    arr[20] = checkSum >> 8;
    arr[21] = checkSum;

    arr[22] = 0x55;
    arr[23] = 0xBB;

}

/**
 * @description: 将数组转换为位姿数据 PC
 * @param {CP_DATA} SLAM 和底盘的位姿 {unsigned char} 输入的字节数组
 * @return: 数据较验是否成功
 */
bool Arr2PosDataPC(CP_DATA &get, unsigned char *arr)
{
    if(arr[0] != 0x55)
        return false;
    if(arr[1] != 0x55)
        return false;
    if(arr[2] != 0xAA)
        return false;
    if(arr[3] != 0xAA)
        return false;

    if(arr[22] != 0x55)
        return false;
    if(arr[23] != 0xBB)
        return false;
    
    int checkSum = 0;
    for(int i = 4; i < 19 + 1; i++)
    {
        checkSum += arr[i];
    }

    int checkSum2Arr = arr[20] << 8 | arr[21];

    if(checkSum != checkSum2Arr)
    {
        cout << "checkSum:" << checkSum << "!=" << checkSum2Arr << endl;
        return false;
    }
    
    // cout << "check Pass" << endl;
    get.SLAM.x = CovUChar2Int(arr[4], arr[5]);
    get.SLAM.y = CovUChar2Int(arr[6], arr[7]);
    get.SLAM.z = CovUChar2Int(arr[8], arr[9]);
    get.SLAM.yaw = CovUChar2Int(arr[10], arr[11]);

    get.Chassis.x = CovUChar2Int(arr[12], arr[13]);
    get.Chassis.y = CovUChar2Int(arr[14], arr[15]);
    get.Chassis.yaw = CovUChar2Int(arr[16], arr[17]);

    get.SLAM.status = arr[18];
    get.Chassis.status = arr[19];

    return true;
}

/**
 * @description: 将数组转换为位姿数据 Chassis
 * @param {CP_DATA} SLAM 和底盘的位姿 {unsigned char} 输入的字节数组
 * @return: 数据较验是否成功
 */
bool Arr2PosChassis(POS &pos, unsigned char *arr)
{
    if(arr[0] != 0x55)
        return false;
    if(arr[1] != 0x55)
        return false;
    if(arr[2] != 0xAA)
        return false;
    if(arr[3] != 0xAA)
        return false;

    if(arr[14] != 0x55)
        return false;
    if(arr[15] != 0xBB)
        return false;
    
    int checkSum = 0;
    for(int i = 4; i < 11 + 1; i++)
    {
        checkSum += arr[i];
    }

    int checkSum2Arr = arr[12] << 8 | arr[13];

    if(checkSum != checkSum2Arr)
    {
        cout << "checkSum:" << checkSum << "!=" << checkSum2Arr << endl;
        return false;
    }
    
    // cout << "check Pass" << endl;
    int x = CovUChar2Int(arr[4], arr[5]);
    int y = CovUChar2Int(arr[6], arr[7]);

    // pos.x = -y;
    // pos.y = x;

    pos.x = x;
    pos.y = y;
    
    pos.yaw = CovUChar2Int(arr[8], arr[9]);

    pos.status = arr[10];
    pos.status_ = arr[11];

    return true;
}

/**
 * @description: 显示位姿数据
 * @param {CP_DATA} CP_DATA的位姿
 * @return: 
 */
void ShowPosPC(CP_DATA get)
{
    cout << "SLAM X : " << get.SLAM.x << endl;
    cout << "SLAM Y : " << get.SLAM.y << endl;
    cout << "SLAM Z : " << get.SLAM.z << endl;
    cout << "SLAM Yaw : " << get.SLAM.yaw << endl;

    cout << "Chassis X : " << get.Chassis.x << endl;
    cout << "Chassis Y : " << get.Chassis.y << endl;
    cout << "Chassis Z : " << get.Chassis.z << endl;
    cout << "Chassis Yaw : " << get.Chassis.yaw << endl;

    cout << hex << "SLAM Status : " << (int)get.SLAM.status << endl;
    cout << hex << "Chassis Status : " << (int)get.Chassis.status << endl;

    cout << dec << endl;

}

/**
 * @description: 显示位姿数据
 * @param {CP_DATA} pos的位姿
 * @return: 
 */
void ShowPosChassis(POS pos)
{
    cout << "X : " << pos.x << endl;
    cout << "Y : " << pos.y << endl;
    cout << "Z : " << pos.z << endl;
    cout << "Yaw : " << pos.yaw << endl;

    cout << hex << "Status : " << (int)pos.status << endl;
    cout << hex << "Status_ : " << (int)pos.status_ << endl;

    cout << dec << endl;

}

/**
 * @description: 显示数组hex形态
 * @param {unsigned char} 需要显示的数组 {int}数组长度 
 * @return: 
 */
void ShowHexArr(unsigned char arr[], int len)
{
    // int len = sizeof(arr) / sizeof(int);

    for(int i = 0; i < len; i ++)
    {
        cout << hex << (int)arr[i] << ", ";
        // printf("%02X, ", arr[i]);
    }

    cout << dec << endl;
    cout << "len is " << len << endl;
}