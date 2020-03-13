###
 # @Author: DahlMill
 # @Date: 2020-03-05 17:50:10
 # @LastEditors: DahlMill
 # @LastEditTime: 2020-03-13 10:17:17
 # @FilePath: /VSLAM/mr133build.sh
 ###

echo "Configuring and building Thirdparty/DBoW2 ..."

cd Thirdparty/DBoW2

sh build.sh

cd ../g2o

echo "Configuring and building Thirdparty/g2o ..."

sh build.sh

cd ../../

export R328_TOOL_ROOT=/home/dm/tina3.5.1/prebuilt/gcc/linux-x86/arm/
export STAGING_DIR=/home/dm/ubuntu/

echo "PATH"
echo $PATH
echo "PATH"

sudo cmake -DCMAKE_TOOLCHAIN_FILE=./platforms/linux/all-winner-r328.cmake
cmake .
# cp makefile_old/CMakeCache.txt ./
sudo make
sudo chmod 777 mono
sudo chmod 777 producer
sudo chmod 777 consumer
sudo chmod 777 interaction

cp mono ~/temp/SLAM/
cp interaction ~/temp/SLAM/
cp chmod_SLAM.sh ~/temp/SLAM/
cp run_mono.sh ~/temp/SLAM/
cp wifi_connect.sh ~/temp/SLAM/
cp WifiConfig  ~/temp/SLAM/

#cp producer ~/temp/SLAM/
#cp consumer ~/temp/SLAM/

cp lib/libSLAM.so ~/temp/lib/
cp Thirdparty/DBoW2/lib/libDBoW2.so ~/temp/lib/
cp Thirdparty/g2o/lib/libg2o.so ~/temp/lib/

cp lib/libSLAM.so /mnt/hgfs/hgfs/
cp ./mono /mnt/hgfs/hgfs/
cp ./interaction /mnt/hgfs/hgfs/
