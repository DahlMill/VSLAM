export R328_TOOL_ROOT=/home/dm/tina3.5.1/prebuilt/gcc/linux-x86/arm/

echo "PATH"
echo $PATH
echo "PATH"

cmake -DCMAKE_TOOLCHAIN_FILE=./platforms/linux/all-winner-r328.cmake
sudo make -j8
