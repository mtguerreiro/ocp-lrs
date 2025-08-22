import time
import datetime
import numpy as np

import pyocp
from . import common

def run_ref_step(fsbb, model_params, exp_params, plat_params, save=False, ctl='energy', k=1.0):

    common.init(fsbb, model_params, exp_params, plat_params)
    
    status = common.enable_cs(fsbb)
    if status != 0:
        print('Failed to enable hw...')
        return (-1, -1)

    common.init_relays(fsbb)

    common.ramp_duty_up(fsbb)
    time.sleep(1 * k)

    if ctl == 'energy':
        fsbb.boost_energy.enable()
    elif ctl == 'energy_mpc':
        fsbb.boost_energy_mpc.enable()
    elif ctl == 'cpl':
        fsbb.cpl.enable()
    elif ctl == 'sfb':
        fsbb.buck_sfb.enable()
    time.sleep(1 * k)

    fsbb.set_ref(exp_params['v_ref_step_up'])
    time.sleep(1 * k)
    
    common.wait_for_trigger(fsbb)

    fsbb.set_ref(exp_params['v_ref'])
    time.sleep(1 * k)
    
    common.ramp_duty_down(fsbb)
    time.sleep(1 * k)
    
    fsbb.idle.enable()
    fsbb.disable()

    status, data = fsbb.trace.read()
        
    return data
