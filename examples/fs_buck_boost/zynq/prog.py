import xsdb
import time

# --- Input ---
ws_path = './build'

# --- LRSSOC app ---
platform_name = 'lrssoc'

cpu0_name = 'cpu0'
cpu0_app_name = f'{cpu0_name}_app'

cpu1_name = 'cpu1'
cpu1_app_name = f'{cpu1_name}_app'

# --- Binaries ---
fpga_bs = f'{ws_path}/{platform_name}/hw/pynq_lrssoc.bit'
fsbl = f'{ws_path}/{platform_name}/zynq_fsbl/build/fsbl.elf'
cpu0_elf = f'{ws_path}/{cpu0_app_name}/build/{cpu0_app_name}.elf'
cpu1_elf = f'{ws_path}/{cpu1_app_name}/build/{cpu1_app_name}.elf'

# --- Program ---
s = xsdb.start_debug_session()
s.connect()

s.targets("--set", filter="name =~ APU")
s.rst()
s.fpga(file=fpga_bs)

s.targets(2)
s.dow(fsbl)
time.sleep(0.25)
s.con()
time.sleep(0.25)
s.stop()
time.sleep(0.25)
s.dow(cpu0_elf)
time.sleep(0.25)
#s.con()

s.targets(3)
time.sleep(0.25)
s.con()
time.sleep(0.25)
s.stop()
time.sleep(0.25)
s.dow(cpu1_elf)
time.sleep(0.25)
#s.con()

s.targets(2)
s.con()

s.targets(3)
s.con()
