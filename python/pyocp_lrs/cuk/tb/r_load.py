import time
import datetime
import numpy as np

import pyocp
from . import common

def run_ref_step(cuk, model_params, exp_params, plat_params, save=False, ctl='energy', k=1.0):

    common.init_cuk(cuk, model_params, exp_params, plat_params)
    
    status = common.enable_cs(cuk)
    if status != 0:
        print('Failed to enable hw...')
        return (-1, -1)

    common.ramp_duty_up(cuk)
    time.sleep(0.5 * k)

    if ctl == 'energy':
        cuk.energy.enable()
    elif ctl == 'energy_mpc':
        cuk.energy_mpc.enable()
    time.sleep(0.5 * k)

##    cuk.set_ref(exp_params['v_ref_step_up'])
##    time.sleep(10)
##    
##    common.wait_for_trigger(cuk)
##
##    cuk.set_ref(exp_params['v_ref'])
##    time.sleep(1)
    
    common.ramp_duty_down(cuk)
    time.sleep(0.5 * k)
    
    cuk.idle.enable()
    cuk.disable()

    status, data = cuk.trace.read()
        
    return data
