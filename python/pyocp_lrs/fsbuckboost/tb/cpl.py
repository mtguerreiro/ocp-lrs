import time
import datetime
import numpy as np

import pyocp
from . import common

def run_ref_step(
    src, src_model_params, src_exp_params, src_plat_params, src_ctl,
    cpl, cpl_model_params, cpl_exp_params, cpl_plat_params, cpl_ctl,
    save=False
    ):

    common.init(src, src_model_params, src_exp_params, src_plat_params)
    common.init(cpl, cpl_model_params, cpl_exp_params, cpl_plat_params)
    
    status = common.enable_cs(src)
    if status != 0:
        print('Failed to enable source converter...')
        return (-1, -1)

    status = common.enable_cs(cpl)
    if status != 0:
        print('Failed to enable cpl...')
        return (-1, -1)
    
    common.init_relays(src)
    time.sleep(1)
    common.init_relays(cpl)
    
    common.ramp_duty_up(src)
    time.sleep(1)
    common.ramp_duty_up(cpl)
    time.sleep(1)

    cpl.cpl.enable()
    time.sleep(1)

##    cpl.trace.set_mode(0)
##    cpl.trace.set_size(50000)
##
##    src.trace.set_mode(0)
##    src.trace.set_size(50000)
##
##    cpl.trace.reset()
##    src.trace.reset()

    if src_ctl == 'energy':
        src.boost_energy.enable()
    elif src_ctl == 'energy_mpc':
        src.boost_energy_mpc.enable()
    time.sleep(1)

    cpl.trace.reset()
    src.trace.reset()
    time.sleep(1)
    
    cpl.set_ref(cpl_exp_params['v_ref_step_up'])
    time.sleep(1)

    src.set_ref(src_exp_params['v_ref_step_up'])
    time.sleep(1)

    src.set_ref(src_exp_params['v_ref'])
    time.sleep(1)
    
    cpl.set_ref(cpl_exp_params['v_ref'])
    time.sleep(1)
    
    common.ramp_duty_down(cpl)
    time.sleep(1)
    common.ramp_duty_down(src)

    cpl.idle.enable()
    cpl.disable()
    
    cpl.idle.enable()
    cpl.disable()

    time.sleep(5)    
    status, src_data = src.trace.read()
    status, cpl_data = cpl.trace.read()
        
    return src_data, cpl_data
