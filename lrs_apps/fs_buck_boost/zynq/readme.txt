/tools/Xilinx/Vitis/2024.2/bin/bootgen -image /home/marco/projects/ocp-lrs/examples/fs_buck_boost/zynq/fsbb.bif -arch zynq -o /home/marco/projects/ocp-lrs/examples/fs_buck_boost/zynq/build/fsbb.bin -w on

program_flash -f /home/marco/projects/ocp-lrs/examples/fs_buck_boost/zynq/build/fsbb.bin -offset 0 -flash_type qspi-x4-single -fsbl /home/marco/projects/ocp-lrs/examples/fs_buck_boost/zynq/build/lrssoc/zynq_fsbl/build/fsbl.elf -url TCP:127.0.0.1:3121
