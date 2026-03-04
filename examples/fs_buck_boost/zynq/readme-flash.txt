/tools/Xilinx/Vitis/2024.2/bin/bootgen -image ./fsbb.bif -arch zynq -o ./fsbb.bin -w on

program_flash -f ./fsbb.bin -offset 0 -flash_type qspi-x4-single -fsbl ./build/lrssoc/zynq_fsbl/build/fsbl.elf -url TCP:127.0.0.1:3121
