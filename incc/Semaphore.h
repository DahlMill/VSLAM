/*
 * @Author: DahlMill
 * @Date: 2020-03-04 10:56:13
 * @LastEditTime: 2020-03-05 10:17:03
 * @LastEditors: Please set LastEditors
 * @Description: 
 * @FilePath: /shmYuv/Semaphore.h
 */

#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int SetSemValue(int iSemID, int value);
void DelSemValue(int iSemID);
int SemaphoreP(int iSemID);
int SemaphoreV(int iSemID);

#endif