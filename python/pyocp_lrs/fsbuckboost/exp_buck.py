import time
import datetime
import numpy as np

import pyocp
import pyocp.data_mng_util as dmu


def config_sfb_controller(fsbb, model_params, ctl_params):
    
    f_pwm = model_params['f_pwm']
    fsbb.hw.set_pwm_frequency(f_pwm)
    
    mp = pyocp.lrs.fsbuckboost.controllers.ModelParams()
    mp.v_in = model_params['V_in']
    mp.R = model_params['R_load']
    mp.L = model_params['L']
    mp.Co = model_params['C_out']
    fsbb.buck_sfb.set_model_params(mp)

    ts = ctl_params['ts']
    os = ctl_params['os']
    dt = 1 / f_pwm
    fsbb.buck_sfb.set_gains(ts=ts, os=os, dt=dt)

    
def config_meas_gains(fsbb, meas_gains):

    fsbb.hw.set_meas_gains(meas_gains)


def init_relays(fsbb):

    fsbb.hw.set_input_relay(1)
    time.sleep(0.25)
    fsbb.hw.set_output_relay(1)


def de_init_bb_relays(fsbb):

    fsbb.hw.set_input_relay(0)
    time.sleep(0.25)
    fsbb.hw.set_output_relay(0)


def enable_cs(fsbb):

    # Procedure to enable the controller. If the hardware is run for the first
    # time, the adc will give an invalid measurement that will trigger an error.
    # This procedure enables/disables/enables the controller as a work-around.
    fsbb.idle.enable()

    fsbb.enable()
    fsbb.disable()
    fsbb.hw.clear_status()

    fsbb.idle.enable()
    fsbb.enable()
    time.sleep(0.1)
    status, hw_status = fsbb.hw.get_status()
    if hw_status != 0:
        print('Hw status is set after enabling...')
        return -1

    return 0


def disable_cs(fsbb):

    fsbb.idle.enable()
    fsbb.disable()


def wait_for_trigger(fsbb):

    status, mode = fsbb.trace.get_mode()
    if status != 0: return
    if mode == 0: return
    
    while True:
        time.sleep(1)
        status, trig_state = fsbb.trace.get_trig_state()
        if trig_state == 4: break


def save_data(fsbb, plat, data, meta):

    status, traces = fsbb.trace.get_signals()

    meta['traces'] = traces
    
    fname = f'{plat}_{datetime.datetime.now().strftime("%Y%m%d_%H%M%S")}'
    ds = dmu.DataSet()
    ds.data = data
    ds.meta = meta
    ds.source = plat

    dmu.save_data(fname, ds)


def run_ref_step(settings, run_params, save=False):

    fsbb = pyocp.lrs.fsbuckboost.iface.Interface('ethernet', settings, cs_id=0, tr_id=0)

    fsbb.set_converter_mode('buck')

    config_sfb_controller(
        fsbb,
        run_params['fsbb']['model_params'],
        run_params['fsbb']['ctl_params']
        )

    config_meas_gains(fsbb, run_params['fsbb']['meas_gains'])

    exp_params = run_params['fsbb']['exp_params']
    
    # Trace config
    fsbb.trace.set_n_pre_trig_samples(100)
    fsbb.trace.set_size(1000)
    
    fsbb.trace.set_trig_level(7)
    fsbb.trace.set_trig_signal(8)

    fsbb.trace.set_mode(1)
    fsbb.trace.reset()
    
    status = enable_cs(fsbb)
    if status != 0:
        print('Failed to enable hw...')
        return (-1, -1)

    init_relays(fsbb)
        
    fsbb.set_ref(3)
    fsbb.buck_sfb.enable()
    time.sleep(0.2)

    fsbb.set_ref(exp_params['v_ref'])

    while True:
        time.sleep(1)
        status, trig_state = fsbb.trace.get_trig_state()
        if trig_state == 4: break

    fsbb.set_ref(3)
    time.sleep(0.2)
    
    fsbb.idle.enable()
    fsbb.disable()

    status, data = fsbb.trace.read()
        
    return data
