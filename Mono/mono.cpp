#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include "Semaphore.h"
#include "Converter.h"
#include <ConvertCP.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "ShmCom.h"
#include <System.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>

#define SEM_SEED 1773
#define SHM_SEED 1774

int iSemID; // Semaphore id
int iShmID; // shared-memory id
void *pSharedMemory = (void *)0;
SHM_POS *pSharedStuff;

#define SEM_SEED_CHASSIS 1333
#define SHM_SEED_CHASSIS 1334

int iSemID2Chassis; // Semaphore id
int iShmID2Chassis; // shared-memory id
void *sharedMemory2Chassis = (void *)0;
SHM_POS *sharedStuff2Chassis;

using namespace cv;
using namespace std;
using namespace ORB_SLAM2;

#define CV_IMG_COUNT 0

double GetCurrentTime(void);

void InitShm(void);
void PostShm(POS pos);

void ShmEnd(void);

void InitShm2Chassis(void);
POS GetShm(void);

int main(int argc, char **argv)
{
    cout << endl
         << "-------mono v2.1-------" << endl;

    InitShm();
    // InitShm2Chassis();

    if (argc != 4)
    {
        cerr << endl
             << "Usage: ./mono_tum path_to_vocabulary path_to_settings path_to_sequence" << endl;
        return 1;
    }

    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ORB_SLAM2::System SLAM(argv[1], argv[2], ORB_SLAM2::System::MONOCULAR, false);

    cout << endl
         << "CV_IMG_COUNT " << CV_IMG_COUNT << endl;
    int iCount = 0;

    // Main loop
    cv::Mat im;
    POS posSLAM;
    memset(&posSLAM, 0, sizeof(POS));

    // ofstream os;                                   //创建一个文件输出流对象
    // os.open("imgLog.txt", ios::out | ios::trunc);  //将对象与文件关联
    // os << "CV_IMG_COUNT " << CV_IMG_COUNT << '\n'; //将输入的内容放入txt文件中

#if CV_IMG_COUNT
    for (int i = 0; i < CV_IMG_COUNT; i++)
#else
    VideoCapture inputVideo(0);
    // inputVideo.set(CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));//视频流格式
    // inputVideo.set(CV_CAP_PROP_FPS, 60);//帧率
    // inputVideo.set(CV_CAP_PROP_FRAME_WIDTH, 640); //帧宽
    // inputVideo.set(CV_CAP_PROP_FRAME_HEIGHT, 480);//帧高

    while (1)
#endif
    {
        ostringstream strPath;
#if CV_IMG_COUNT
        strPath << "test/img" << iCount << ".jpg";
        im = imread(strPath.str());
#else
        inputVideo >> im;
#endif
        // timeval tv;
        // gettimeofday(&tv, NULL);
        double tframe = GetCurrentTime();

        if (im.empty())
        {
            cout << "Failed to load image" << endl;
            return 1;
        }

        // Pass the image to the SLAM system
        // double dbTime = tframe;
        Mat Tcw = SLAM.TrackMonocular(im, tframe);

        if ((Tcw.cols == 4) && (Tcw.rows == 4))
        {
            Mat Rwc = Tcw.rowRange(0, 3).colRange(0, 3).t();
            Mat Twc = -Rwc * Tcw.rowRange(0, 3).col(3);
            vector<float> q = Converter::toQuaternion(Rwc);
            // cout << "------------------" << endl;
            // cout << Twc << endl;

            float fx = Twc.at<float>(0, 0) * 100;
            float fy = Twc.at<float>(1, 0) * 100;
            float fz = Twc.at<float>(2, 0) * 100;

            cout << setprecision(12)
                 << "[" << tframe << "] SLAM X " << fx << " Y " << fy << " Z " << fz << endl;
            posSLAM.x = fx;
            posSLAM.y = fy;
            posSLAM.z = fz;

            cout << "------------------" << endl;
        }

        posSLAM.imgCount = iCount;
        PostShm(posSLAM);

#if CV_IMG_COUNT
#else
        // POS posChassis = GetShm();
        // cout << "Chassis " << "X " << posChassis.x << " Y " << posChassis.y << " Yaw " << posChassis.yaw << endl;
        iCount++;
        strPath << "/tmp/img" << iCount << ".jpg";
        cout << setprecision(12)
             << "[" << tframe << "] write img " << strPath.str() << endl;
        imwrite(strPath.str(), im);

        // ostringstream strLog;
        // strLog << iCount << " " << tframe << " " << posChassis.x << " " << posChassis.y << " " << posChassis.yaw << '\n';

        // os << strLog.str(); //将输入的内容放入txt文件中
        // os.flush();

        cout << "------------------" << endl;
#endif
    }

    // Stop all threads
    SLAM.Shutdown();

    ShmEnd();

    // os.close();

    // Save camera trajectory
    // SLAM.SaveKeyFrameTrajectoryTUM("KeyFrameTrajectory.txt");

    exit(EXIT_SUCCESS);

    return 0;
}

POS GetShm(void)
{
    POS pos;
    SemaphoreP(iSemID2Chassis);
    memcpy(&pos, (char *)sharedStuff2Chassis->shmSp, sizeof(POS));
    // iEndFlag = sharedStuff2Chassis->endFlag;
    SemaphoreV(iSemID2Chassis);
    return pos;
}

void ShmEnd(void)
{
    // SemaphoreP(iSemID);
    // pSharedStuff->endFlag = 1;
    // SemaphoreV(iSemID);

    DelSemValue(iSemID);
    if (shmdt(pSharedMemory) == -1)
    {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }
}

void PostShm(POS pos)
{
    // cout << "SemaphoreP" << endl;
    SemaphoreP(iSemID);
    /* Write it into shared memory */
    // cout << "memcpy" << endl;
    memcpy((char *)pSharedStuff->shmSp, &pos, sizeof(POS));
    // pSharedStuff->endFlag = 0;
    // cout << "SemaphoreV" << endl;
    SemaphoreV(iSemID);
}

void InitShm(void)
{
    /* Create and init Semaphore */
    cout << "InitShm" << endl;
    iSemID = semget((key_t)SEM_SEED, 1, 0666 | IPC_CREAT);
    if (iSemID == -1)
    {
        fprintf(stderr, "semget failed\n");
        exit(EXIT_FAILURE);
    }

    /* Init shared-memory */
    iShmID = shmget((key_t)SHM_SEED, sizeof(SHM_POS), 0666 | IPC_CREAT);
    if (iShmID == -1)
    {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }

    pSharedMemory = shmat(iShmID, (void *)0, 0);
    if (pSharedMemory == (void *)-1)
    {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }

    pSharedStuff = (SHM_POS *)pSharedMemory;
    pSharedStuff->endFlag = 0;

    SetSemValue(iSemID, 1);
}

void InitShm2Chassis(void)
{
    cout << "InitShm2Chassis" << endl;
    /* Create and init Semaphore */
    iSemID2Chassis = semget((key_t)SEM_SEED_CHASSIS, 1, 0666 | IPC_CREAT);
    if (iSemID2Chassis == -1)
    {
        fprintf(stderr, "semget failed\n");
        exit(EXIT_FAILURE);
    }

    /* Init shared-memory */
    iShmID2Chassis = shmget((key_t)SHM_SEED_CHASSIS, sizeof(SHM_POS), 0666 | IPC_CREAT);
    if (iShmID2Chassis == -1)
    {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }

    sharedMemory2Chassis = shmat(iShmID2Chassis, (void *)0, 0);
    if (sharedMemory2Chassis == (void *)-1)
    {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }

    sharedStuff2Chassis = (SHM_POS *)sharedMemory2Chassis;
    sharedStuff2Chassis->endFlag = 0;

    SetSemValue(iSemID2Chassis, 1);
}

double GetCurrentTime(void)
{
    timeval tv;
    gettimeofday(&tv, NULL);

    double second = tv.tv_sec % 1000000000;
    long usecond = tv.tv_usec / 1000;

    return second + (usecond / 1000.0000f);

}