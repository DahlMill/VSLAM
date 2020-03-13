/*
 * @Author: DahlMill
 * @Date: 2020-03-04 10:49:34
 * @LastEditTime: 2020-03-04 16:11:19
 * @LastEditors: DahlMill
 * @Description: 
 * @FilePath: /shmYuv/shm_com.h
 */

#ifndef __SHM_COM_H__
#define __SHM_COM_H__


#define MAX_SHM_SIZE 2048 * 2048 * 3

typedef struct shared_use_st
{
    int end_flag;              //用来标记进程间的内存共享是否结束: 0, 未结束； 1， 结束
    char shm_sp[MAX_SHM_SIZE]; //共享内存的空间
} shared_use_st;

typedef struct STU_SHM_POS
{
    int endFlag;              //用来标记进程间的内存共享是否结束: 0, 未结束； 1， 结束
    char shmSp[64]; //共享内存的空间
} SHM_POS;

#endif