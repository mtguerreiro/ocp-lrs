import pyocp_lrs
import pyocp.data_mng_util as dmu

import numpy as np
import matplotlib.pyplot as plt
plt.ion()

import time

import os
import plecsutil as pu
import plecs.model

# --- Input ---
# Plecs model
plecs_file = 'plecs/lrs_fs_buck_boost'
plecs_file_path = os.path.abspath(os.getcwd())

# Controller interface

port_hw = 8080
host_hw = '131.246.75.251'
settings_hw = {'host':host_hw, 'port':port_hw}

port_sim = 8080
host_sim = 'localhost'
settings_sim = {'host':host_sim, 'port':port_sim}

bb_sim = pyocp_lrs.fsbuckboost.iface.Interface('ethernet', settings_sim, cs_id=0, tr_id=0)
bb_hw = pyocp_lrs.fsbuckboost.iface.Interface('ethernet', settings_hw, cs_id=0, tr_id=0)


# --- Functions ---
def config_sim(model_params):
    
    pm = pu.ui.PlecsModel(
        plecs_file, plecs_file_path,
        plecs.model.params(),
        )

    pm.gen_m_file(sim_params=model_params)


def run(settings, save=False):        

    run_params = dmu.load_json('run_params')
    
    # Sets the sim
    if settings['host'] == 'localhost':
        model_params = run_params['fsbb']['model_params']
        config_sim(model_params)
        input('Simulation configured. Press any key to continue...')
        
    data = pyocp_lrs.fsbuckboost.exp_boost.run_ref_step(settings, run_params, save=save)

    return data
