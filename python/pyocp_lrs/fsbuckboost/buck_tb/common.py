import time
import datetime
import numpy as np

import pyocp
import pyocp.data_mng_util as dmu


def init(fsbb, params):

    fsbb.disable()
    fsbb.idle.enable()
    fsbb.set_ref(params['exp_params']['v_ref'])

    fsbb.set_converter_mode('buck')

    config_cpl_controller(
        fsbb,
        params['model_params'],
        params['cpl_params']
    )
    fsbb.buck_sfb.reset()
    fsbb.cpl.reset()

    config_ramp_controller(fsbb, params['ramp_params'])
    fsbb.ramp.reset()
    
    config_meas_gains(fsbb, params['meas_gains'])

    fsbb.trace.set_n_pre_trig_samples(100)
    fsbb.trace.set_size(1000)

    v_ref = params['exp_params']['v_ref']
    v_ref_step_up = params['exp_params']['v_ref_step_up']
    trig_level = (v_ref + v_ref_step_up) / 2
    fsbb.trace.set_trig_level(trig_level)
    fsbb.trace.set_trig_signal(8)

    fsbb.trace.set_mode(1)
    fsbb.trace.reset()


def config_cpl_controller(fsbb, model_params, ctl_params):
    
    f_pwm = model_params['f_pwm']
    fsbb.hw.set_pwm_frequency(f_pwm)

    ts = ctl_params['ts']
    os = ctl_params['os']
    dt = 1 / f_pwm
    fsbb.cpl.set_gains(ts=ts, os=os, dt=dt)

    v_min = ctl_params['v_min']
    v_max = ctl_params['v_max']
    
    fsbb.cpl.set_params({'v_min':v_min, 'v_max':v_max})


def config_ramp_controller(fsbb, ctl_params):

    fsbb.ramp.set_params({
        'u_step':ctl_params['u_step'],
        'u_ref':ctl_params['u_ref']
    })
    
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


def ramp_duty_up(fsbb, ramp_params):

    fsbb.ramp.reset()
    fsbb.ramp.enable()
    time.sleep(0.1)


def ramp_duty_down(fsbb):

    fsbb.ramp.set_params({'u_ref':0})
    fsbb.ramp.enable()
    time.sleep(0.1)


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
