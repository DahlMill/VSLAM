#-----------------------------------------------------------------------------------------
# R328 Toolchain CMake
# by zhangzw
#
# need set sys env R6_TOOL_ROOT
# 1. edit ./bashrc add env R328_TOOL_ROOT
# ex:
# export R328_TOOL_ROOT=/home/fz/projects/build-tools/R328
#
#-----------------------------------------------------------------------------------------

#set(R328_TOOLCHAIN /mnt/server/rxx_tina3.0_git/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-musl/toolchain)

set(R328_TOOLCHAIN $ENV{R328_TOOL_ROOT}/toolchain-sunxi-musl/toolchain)

set(CMAKE_SYSROOT ${R328_TOOLCHAIN})

set(CMAKE_SYSTEM_NAME Linux)

set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER ${R328_TOOLCHAIN}/bin/arm-openwrt-linux-gcc CACHE FILEPATH "allwinner r328 gcc")

set(CMAKE_CXX_COMPILER ${R328_TOOLCHAIN}/bin/arm-openwrt-linux-g++ CACHE FILEPATH "allwinner r328 g++")


SET(CMAKE_FIND_ROOT_PATH ${R328_TOOLCHAIN} ${R328_TOOLCHAIN}/stagging)

set(OpenCV_DIR "/mnt/server/rxx_tina3.0_git_qingping_20190822/out/drum-std_xr829/staging_dir/target/usr/share/OpenCV" CACHE FILEPATH "allwinner cmake lib")

# set(CMAKE_FIND_ROOT_PATH /opt/gcc-linaro-7.2.1-2017.11-x86_64_arm-linux-gnueabihf)

#set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
#set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
#set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

#-----------------------------------------------------------------------------------------

