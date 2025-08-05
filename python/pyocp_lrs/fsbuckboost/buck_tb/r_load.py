import time
import datetime
import numpy as np

import pyocp
from . import common

def run_ref_step(fsbb, run_params, save=False, ctl='cpl'):

    common.init(fsbb, run_params)

    exp_params = run_params['exp_params']
    ramp_params = run_params['ramp_params']
    
    status = common.enable_cs(fsbb)
    if status != 0:
        print('Failed to enable hw...')
        return (-1, -1)

    common.init_relays(fsbb)

    common.ramp_duty_up(fsbb, ramp_params)
    time.sleep(1)

    if ctl == 'cpl':
        fsbb.cpl.enable()
    else:
        fsbb.buck_sfb.enable()
    time.sleep(1)

    fsbb.set_ref(exp_params['v_ref_step_up'])
    time.sleep(1)
    
    while True:
        time.sleep(1)
        status, trig_state = fsbb.trace.get_trig_state()
        if trig_state == 4: break

    fsbb.set_ref(exp_params['v_ref'])
    time.sleep(1)
    
    common.ramp_duty_down(fsbb)
    time.sleep(1)
    
    fsbb.idle.enable()
    fsbb.disable()

    status, data = fsbb.trace.read()
        
    return data
