/*
 * @Author: DahlMill
 * @Date: 2020-03-04 10:58:43
 * @LastEditTime: 2020-03-04 16:27:01
 * @LastEditors: DahlMill
 * @Description: 
 * @FilePath: /shmYuv/producer.cpp
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

    /* Create and init Semaphore */
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
    pSharedStuff->endFlag = 0;

    printf("FRAME_CNT: %d\n", frame_cnt);
    SetSemValue(iSemID, 1);

    POS pos;
    pos.x = 1;
    pos.y = 2;
    pos.z = 3;
    pos.yaw = 4;
    pos.status = 5;

    while (frame_cnt < 1000)
    {
        SemaphoreP(iSemID);

        /* Write it into shared memory */
        memcpy((char *)pSharedStuff->shmSp, &pos, sizeof(POS));

        pSharedStuff->endFlag = 0;

        SemaphoreV(iSemID);

        pos.x ++;
        pos.y ++;
        pos.z ++;
        pos.yaw ++;
        pos.status ++;

        frame_cnt++;

        usleep(10 * 1000);

        printf("FRAME_CNT: %d\n", frame_cnt);
    }
    SemaphoreP(iSemID);
    pSharedStuff->endFlag = 1;
    SemaphoreV(iSemID);

    /* over */
    printf("\nProducer over!\n");
    DelSemValue(iSemID);
    if (shmdt(pSharedMemory) == -1)
    {
        fprintf(stderr, "shmdt failed.\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}