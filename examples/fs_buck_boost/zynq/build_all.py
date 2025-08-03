import vitis
import os
import pathlib
import shutil
import zipfile

# --- Input ---
ws_path = './build'

ocp_lrs = '/home/marco/projects/ocp-lrs'
lrs_app = 'fs_buck_boost'

hw_file_name = 'pynq_lrssoc'
hw_file_path = '/home/marco/projects/ocp-lrs/lrs_apps/fs_buck_boost/zynq'
hw_file = f'{hw_file_path}/{hw_file_name}.xsa'

# --- LRSSOC app ---
platform_name = 'lrssoc'

cpu0_name = 'cpu0'
cpu0_app_name = f'{cpu0_name}_app'

cpu1_name = 'cpu1'
cpu1_app_name = f'{cpu1_name}_app'

# --- Workspace and projects ---
# Create the workspace
pathlib.Path(ws_path).mkdir(exist_ok=True)

# Start the client and sets the worskapce
client = vitis.create_client()
client.set_workspace(ws_path + '/')

# Load/create platform
platform_path = ws_path + '/' + platform_name
if os.path.isdir(platform_path):
    client.add_platform_repos(platform_path)
    platform = client.get_component(platform_name)
    platform.build()

    cpu0_app = client.get_component(cpu0_app_name)
    cpu1_app = client.get_component(cpu1_app_name)
    
else:
    platform = client.create_platform_component(name=platform_name, hw_design=hw_file)

    fsbl = platform.get_domain('zynq_fsbl')
    proc_extra_comp_flags = fsbl.get_config('proc', 'proc_extra_compiler_flags')
    value = proc_extra_comp_flags['value']
    value = value + ' -DUSE_AMP=1'
    fsbl.set_config('proc', 'proc_extra_compiler_flags', value)

    platform.add_domain(name=cpu0_name, cpu='ps7_cortexa9_0', os='freertos')
    cpu0 = platform.get_domain(cpu0_name)
    cpu0.set_lib('lwip220')
    cpu0.set_config('lib', 'lwip220_api_mode', 'SOCKET_API', 'lwip220')
    cpu0.set_config('lib', 'lwip220_dhcp', True, 'lwip220')

    platform.add_domain(name=cpu1_name, cpu='ps7_cortexa9_1', os='standalone')
    cpu1 = platform.get_domain(cpu1_name)
    
    platform.build()

    client.add_platform_repos(platform_path)
    platform_xpfm = client.find_platform_in_repos(platform_name)

    cpu0_app = client.create_app_component(name=cpu0_app_name, platform=platform_xpfm, domain=cpu0_name, template='freertos_lwip_echo_server')
    cpu1_app = client.create_app_component(name=cpu1_app_name, platform=platform_xpfm, domain=cpu1_name, template='hello_world')

# --- OCP-LRS project ---
# Replaces UserConfig.cmake files in project dir
source = f'{ocp_lrs}/examples/{lrs_app}/zynq'

pathlib.Path(f'{ws_path}/{cpu0_app_name}/src/UserConfig.cmake').unlink()
file = 'UserConfigCpu0.cmake'
f = shutil.copy(f'{source}/{file}', f'{ws_path}/{cpu0_app_name}/src')
pathlib.Path(f).rename(f'{ws_path}/{cpu0_app_name}/src/UserConfig.cmake')

pathlib.Path(f'{ws_path}/{cpu1_app_name}/src/UserConfig.cmake').unlink()
file = 'UserConfigCpu1.cmake'
f = shutil.copy(f'{source}/{file}', f'{ws_path}/{cpu1_app_name}/src')
pathlib.Path(f).rename(f'{ws_path}/{cpu1_app_name}/src/UserConfig.cmake')

# Extracts bitstream from xsa file
with zipfile.ZipFile(hw_file, 'r') as f:
    f.extract(f'{hw_file_name}.bit', f'{ws_path}/{platform_name}/hw')

# --- Build applications ---
cpu0_app.clean()
cpu0_app.build()

cpu1_app.clean()
cpu1_app.build()

#vitis.dispose()
