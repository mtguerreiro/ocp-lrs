import time
import datetime
import numpy as np

import pyocp
from . import common

def run_ref_step(fsbb, model_params, exp_params, plat_params, save=False, ctl='energy'):

    common.init(fsbb, model_params, exp_params, plat_params)
    
    status = common.enable_cs(fsbb)
    if status != 0:
        print('Failed to enable hw...')
        return (-1, -1)

    common.init_relays(fsbb)

    common.ramp_duty_up(fsbb)
    time.sleep(1)

    if ctl == 'energy':
        fsbb.boost_energy.enable()
    elif ctl == 'energy_mpc':
        fsbb.boost_energy_mpc.enable()
    elif ctl == 'cpl':
        fsbb.cpl.enable()
    time.sleep(1)

    fsbb.set_ref(exp_params['v_ref_step_up'])
    time.sleep(10)
    
    common.wait_for_trigger(fsbb)

    fsbb.set_ref(exp_params['v_ref'])
    time.sleep(1)
    
    common.ramp_duty_down(fsbb)
    time.sleep(1)
    
    fsbb.idle.enable()
    fsbb.disable()

    status, data = fsbb.trace.read()
        
    return data
