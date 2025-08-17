import time
import datetime
import numpy as np

import pyocp
from . import common

def run_ref_step(
    src, src_model_params, src_exp_params, src_plat_params, src_ctl,
    cpl, cpl_model_params, cpl_exp_params, cpl_plat_params, cpl_ctl,
    save=False, k=1.0
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

    cpl.trace.reset()
    src.trace.reset()
    
    common.init_relays(src)
    time.sleep(0.5 * k)
    common.init_relays(cpl)
    time.sleep(0.5 * k)
    
    common.ramp_duty_up(src)
    time.sleep(0.2 * k)
    common.ramp_duty_up(cpl)
    time.sleep(0.2 * k)

    cpl.cpl.enable()
    time.sleep(0.2 * k)

    if src_ctl == 'energy':
        src.boost_energy.enable()
    elif src_ctl == 'energy_mpc':
        src.boost_energy_mpc.enable()
    time.sleep(0.1 * k)

    cpl.set_ref(cpl_exp_params['v_ref_step_up'])
    time.sleep(0.1 * k)

    src.set_ref(src_exp_params['v_ref_step_up'])
    time.sleep(0.1 * k)

    src.set_ref(src_exp_params['v_ref'])
    time.sleep(0.1 * k)
    
    cpl.set_ref(cpl_exp_params['v_ref'])
    time.sleep(0.1 * k)
    
    common.ramp_duty_down(cpl)
    time.sleep(0.2 * k)
    common.ramp_duty_down(src)
    time.sleep(0.2 * k)

    common.wait_for_trigger(cpl)
    status, cpl_data = cpl.trace.read()
    cpl.idle.enable()
    cpl.disable()
    
    common.wait_for_trigger(src)
    status, src_data = src.trace.read()    
    src.idle.enable()
    src.disable()

    if save:
        src_meta = {
            'model':src_model_params,
            'plat':src_plat_params,
            'exp':src_exp_params,
            'ctl':src_ctl
        }
        common.save_data(src, save + '_src', src_data, src_meta)

        cpl_meta = {
            'model':cpl_model_params,
            'plat':cpl_plat_params,
            'exp':cpl_exp_params,
            'ctl':cpl_ctl
        }
        common.save_data(cpl, save + '_cpl', cpl_data, cpl_meta)
        
    return src_data, cpl_data
