/*
 * @Author: DahlMill
 * @Date: 2020-03-04 10:59:15
 * @LastEditTime: 2020-03-05 10:16:47
 * @LastEditors: Please set LastEditors
 * @Description: 
 * @FilePath: /shmYuv/consumer.cpp
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/shm.h>
#include <sys/sem.h>

#include "Semaphore.h"
#include "ShmCom.h"

#include "ConvertCP.h"

#define SEM_SEED 1000
#define SHM_SEED 1001

int main(int argc, char *argv[])
{
    int frame_cnt = 0;

    int iSemID; // Semaphore id

    int iShmID; // shared-memory id
    void *pSharedMemory = (void *)0;
    SHM_POS *pSharedStuff;
    int end_flag = 0;

    /* Create Semaphore */
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

    printf("FRAME_CNT: %d\n", frame_cnt);
    /* 
       * 必须先置0，
       * 否则会因生产者进程的异常退出未释放信号量而导致程序出错 
       */
    SetSemValue(iSemID, 0);

    POS pos;

    do
    {
        SemaphoreP(iSemID);

        /* Read frame from shared-memory */

        memcpy(&pos, (char *)pSharedStuff->shmSp, sizeof(POS));
        printf("\n");
        printf("X: %d\n", pos.x);
        printf("Y: %d\n", pos.y);
        printf("Z: %d\n", pos.z);
        printf("Yaw: %d\n", pos.yaw);
        printf("Status: %d\n", pos.status);

        end_flag = pSharedStuff->endFlag;

        // crop_frame(&frame, 10, 10, 40, 40);

        SemaphoreV(iSemID);

        frame_cnt++;
        printf("FRAME_CNT: %d\n", frame_cnt);

        usleep(1000 * 1000);
    } while (!end_flag);

    /* Over */
    printf("\nConsumer over!\n");

    if (shmdt(pSharedMemory) == -1)
    {
        fprintf(stderr, "shmdt failed.\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}