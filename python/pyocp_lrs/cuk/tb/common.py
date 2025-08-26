import time
import datetime
import numpy as np

import pyocp
import pyocp.data_mng_util as dmu

from . import controllers

def init_cuk(cuk, model_params, exp_params, plat_params):

    cuk.disable()
    cuk.idle.enable()
    cuk.set_ref(exp_params['v_ref'])

    v_in = model_params['V_in']
    v_ref_ini = exp_params['v_ref']
    f_pwm = model_params['f_pwm']
    nt = model_params['N2'] / model_params['N1']

    cuk.hw.set_pwm_frequency(f_pwm)
    cuk.hw.set_low_pass_filt_coef(plat_params['low_pass_filt_coef'])

    plat_params['ramp_params']['u_ref'] = v_ref_ini / (v_ref_ini + nt * v_in)
    config_ramp_controller(cuk, plat_params['ramp_params'])
    cuk.ramp.reset()

    controllers.config_cuk(cuk, model_params, plat_params)

    #cuk.hw.set_meas_gains(plat_params['meas_gains'])

    cuk.trace.set_n_pre_trig_samples(plat_params['trigger']['pretrig'])
    cuk.trace.set_size(plat_params['trigger']['size'])
    cuk.trace.set_trig_signal(plat_params['trigger']['signal'])
    cuk.trace.set_trig_level(plat_params['trigger']['level'])
    
    cuk.trace.set_mode(1)
    cuk.trace.reset()


def init_cpl(fsbb, model_params, exp_params, plat_params):

    mode = 'buck'

    fsbb.disable()
    fsbb.idle.enable()
    fsbb.set_ref(exp_params['v_ref'])

    v_in = model_params['V_in']
    v_ref_ini = exp_params['v_ref']
    f_pwm = model_params['f_pwm']

    fsbb.hw.set_pwm_frequency(f_pwm)
        
    fsbb.set_converter_mode(mode)
    ramp_u_ref = v_ref_ini / v_in

    plat_params['ramp_params']['u_ref'] = ramp_u_ref
    config_ramp_controller(fsbb, plat_params['ramp_params'])
    fsbb.ramp.reset()

    controllers.config_cpl(fsbb, model_params, plat_params)

    fsbb.hw.set_meas_gains(plat_params['meas_gains'])

    fsbb.trace.set_n_pre_trig_samples(plat_params['trigger']['pretrig'])
    fsbb.trace.set_size(plat_params['trigger']['size'])
    fsbb.trace.set_trig_signal(plat_params['trigger']['signal'])
    fsbb.trace.set_trig_level(plat_params['trigger']['level'])
    
    fsbb.trace.set_mode(1)
    fsbb.trace.reset()


def config_ramp_controller(cuk, ctl_params):

    cuk.ramp.set_params({
        'u_step':ctl_params['u_step'],
        'u_ref':ctl_params['u_ref']
    })


def enable_cs(controller):

    # Procedure to enable the controller. If the hardware is run for the first
    # time, the adc will give an invalid measurement that will trigger an error.
    # This procedure enables/disables/enables the controller as a work-around.
    controller.idle.enable()

    controller.enable()
    controller.disable()
    controller.hw.clear_status()

    controller.idle.enable()
    controller.enable()
    time.sleep(0.1)
    status, hw_status = controller.hw.get_status()
    if hw_status != 0:
        print('Hw status is set after enabling...')
        return -1

    return 0


def disable_cs(controller):

    controller.idle.enable()
    controller.disable()


def init_cpl_relays(fsbb):

    fsbb.hw.set_input_relay(1)
    time.sleep(0.25)
    fsbb.hw.set_output_relay(1)


def de_init_cpl_relays(fsbb):

    fsbb.hw.set_input_relay(0)
    time.sleep(0.25)
    fsbb.hw.set_output_relay(0)


def ramp_duty_up(controller):

    controller.ramp.reset()
    controller.ramp.enable()
    time.sleep(0.1)


def ramp_duty_down(controller):

    controller.ramp.set_params({'u_ref':0})
    controller.ramp.enable()
    time.sleep(0.1)


def wait_for_trigger(controller):

    status, mode = controller.trace.get_mode()
    if status != 0: return
    if mode == 0: return
    
    while True:
        status, trig_state = controller.trace.get_trig_state()
        if trig_state == 4: break
        time.sleep(1)


def save_data(controller, file, data, meta):

    status, traces = controller.trace.get_signals()

    meta['traces'] = traces
    
    fname = f'{file}_{datetime.datetime.now().strftime("%Y%m%d_%H%M%S")}'
    ds = dmu.DataSet()
    ds.data = data
    ds.meta = meta
    ds.source = ''

    dmu.save_data(fname, ds)
