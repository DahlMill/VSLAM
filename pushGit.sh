cd ~/ubuntu/cmake_compile/
mkdir /mnt/hgfs/hgfs/gitBuffer/VSLAM
mkdir /mnt/hgfs/hgfs/gitBuffer/VSLAM/$(date +%Y年%m月)

sudo tar -zcv --exclude='*ORBvoc.txt*' --exclude='*rgbd_dataset_freiburg1_xyz*' -f /mnt/hgfs/hgfs/gitBuffer/VSLAM/$(date +%Y年%m月)/$(date +%d日%H时%M分%S秒).tar.gz ./

#cd /home/dm/ubuntu/pushDir/cmake_compile/
#git add .
#git commit -m "push"
#git push
