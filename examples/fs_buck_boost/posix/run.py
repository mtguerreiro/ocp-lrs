import pyocp_lrs
import pyocp

import numpy as np
import matplotlib.pyplot as plt
plt.ion()

import time
import os, subprocess, platform

import plecsutil as pu
import plecs.model

# --- Input ---
# Plecs model
plecs_file = 'plecs/lrs_fs_buck_boost'
plecs_file_path = os.path.abspath(os.getcwd())

# Controller interface
port = 8080
host = 'localhost'
settings = {'host':host, 'port':port}

fsbb = pyocp_lrs.fsbuckboost.iface.Interface('ethernet', settings, cs_id=0, tr_id=0)

# --- Functions ---
def config_sim(model_params):
    
    pm = pu.ui.PlecsModel(
        plecs_file, plecs_file_path,
        plecs.model.params(),
        )

    pm.gen_m_file(sim_params=model_params)


def make():

    build_path = os.path.abspath(os.getcwd()) + '/build'

    if not os.path.exists(build_path):
        os.makedirs(build_path)
    
    plat = platform.system()

    make_cmd = ['cmake', '..', '-G', 'Ninja']
    build_cmd = 'ninja'

    if plat == 'Linux':
        run_cmd = r'/app_fsbb'
    elif plat == 'Windows':
        run_cmd = r'app_fsbb.exe'
    else:
        raise ValueError('Platform not supported for code generation.')

    subprocess.run(make_cmd, cwd=build_path, check=True)
    subprocess.run([build_cmd], cwd=build_path, check=True)

    subprocess.Popen([build_path + run_cmd], cwd=build_path)    

run_params = pyocp.data_mng_util.load_json('run_params')
#config_sim(run_params['fsbb']['model_params'])
config_sim(run_params['cpl']['model_params'])
make()
