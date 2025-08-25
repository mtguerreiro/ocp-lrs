
def config_cuk(cuk, model_params, plat_params):

    if 'energy_params' in plat_params:
        _config_cuk_energy_controller(
            cuk,
            model_params,
            plat_params['energy_params']
        )

    if 'energy_mpc_params' in plat_params:
        _config_cuk_energy_mpc_controller(
            cuk,
            model_params,
            plat_params['energy_mpc_params']
        )

def _config_cuk_energy_controller(cuk, model_params, ctl_params):
    
    f_pwm = model_params['f_pwm']

    ts = ctl_params['ts']
    os = ctl_params['os']
    dt = 1 / f_pwm
    cuk.energy.set_gains(ts=ts, os=os, dt=dt)

    notch_en = ctl_params['notch_en']
    fc = ctl_params['fc']
    Q = ctl_params['Q']
    cuk.energy.set_notch(fc, Q=Q, dt=dt, enable=bool(notch_en))

    C_out = ctl_params['C_out']
    cuk.energy.set_params({'C_out':C_out})

    cuk.energy.reset()


def _config_cuk_energy_mpc_controller(cuk, model_params, ctl_params):
    
    f_pwm = model_params['f_pwm']
    dt = 1 / f_pwm

    rw = ctl_params['rw']
    l_pred = ctl_params['l_pred']
    alpha = ctl_params['alpha']

    fw = ctl_params['fw']
    qw = ctl_params['qw']
    l_past = ctl_params['l_past']
    window = ctl_params['window']

    il_max = ctl_params['il_max']
    il_min = ctl_params['il_min']
    
    Co = ctl_params['C_out']

    cuk.energy_mpc.set_gains(
        rw=rw, l_pred=l_pred, alpha=alpha,
        fw=fw, qw=qw, l_past=l_past, window=window,
        il_max=il_max, il_min=il_min, Co=Co,
        dt=dt
    )

    cuk.energy_mpc.reset()


def config_cpl(fsbb, model_params, plat_params):

    if 'buck_cpl_params' in plat_params:
        _config_buck_cpl_controller(
            fsbb,
            model_params,
            plat_params['buck_cpl_params']
        )


def _config_buck_cpl_controller(fsbb, model_params, ctl_params):

    f_pwm = model_params['f_pwm']
    dt = 1 / f_pwm
    
    ts = ctl_params['ts']
    os = ctl_params['os']

    _mparams = fsbb.cpl.get_model_params()
    _mparams.v_in = model_params['V_in']
    _mparams.R = model_params['R_load']
    _mparams.L = model_params['L']
    _mparams.Co = model_params['C_out']

    fsbb.cpl.set_model_params(_mparams)

    fsbb.cpl.set_gains(ts=ts, os=os, dt=dt)

    fsbb.cpl.set_params(
        {'v_min':ctl_params['v_min'], 'v_max':ctl_params['v_max']}
    )
    
    fsbb.cpl.reset()
