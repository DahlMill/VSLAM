/*
 * @Author: DahlMill
 * @Date: 2020-03-04 10:59:47
 * @LastEditTime: 2020-03-04 16:22:46
 * @LastEditors: DahlMill
 * @Description: 
 * @FilePath: /shmYuv/SemAphore.cpp
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>

#include "Semaphore.h"

/* init Semaphore by semctl */
int SetSemValue(int iSemID, int value)
{
  union semun sem_union;
    
  sem_union.val = value;
  if (semctl(iSemID, 0, SETVAL, sem_union) == -1)
    return 1;

  return 0;
}


/* delete Semaphore by sectl */
void DelSemValue(int iSemID)
{
  union semun sem_union;

  if (semctl(iSemID, 0, IPC_RMID, sem_union) == -1)
    fprintf(stderr, "Failed to delete Semaphore\n");
}

/**
 * @description: 信号量置位
 * @param {type} 信号量ID
 * @return: 置位情况
 */
int SemaphoreP(int iSemID)
{
  struct sembuf sem_b;
  sem_b.sem_num = 0;
  sem_b.sem_op = -1; /* P(v) */
  sem_b.sem_flg = SEM_UNDO;

  // printf("semop %d\n", sem_id);
  if (semop(iSemID, &sem_b, 1) == -1)
  {
    fprintf(stderr, "SemaphoreP failed\n");
    return 1;
  }
  // printf("semop OK");

  return 0;
}

/**
 * @description: 信号量复位
 * @param {type} 信号量ID
 * @return: 复位情况
 */
int SemaphoreV(int iSemID)
{
  struct sembuf sem_b;

  sem_b.sem_num = 0;
  sem_b.sem_op = 1; // V(v)
  sem_b.sem_flg = SEM_UNDO;

  if (semop(iSemID, &sem_b, 1) == -1)
  {
    fprintf(stderr, "SemaphoreV failed\n");
    return 1;
  }

  return 0;
}