
def config(fsbb, model_params, plat_params):

    if 'buck_cpl_params' in plat_params:
        _config_buck_cpl_controller(
            fsbb,
            model_params,
            plat_params['buck_cpl_params']
        )
        
    if 'boost_energy_params' in plat_params:
        _config_boost_energy_controller(
            fsbb,
            model_params,
            plat_params['boost_energy_params']
        )

    if 'boost_energy_mpc_params' in plat_params:
        _config_boost_energy_mpc_controller(
            fsbb,
            model_params,
            plat_params['boost_energy_mpc_params']
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

def _config_boost_energy_controller(fsbb, model_params, ctl_params):
    
    f_pwm = model_params['f_pwm']

    ts = ctl_params['ts']
    os = ctl_params['os']
    dt = 1 / f_pwm
    fsbb.boost_energy.set_gains(ts=ts, os=os, dt=dt)

    alpha = ctl_params['alpha']
    filt_en = ctl_params['filt_en']
    kd = ctl_params['kd']
    fsbb.boost_energy.set_params({'alpha':alpha, 'filt_en':filt_en, 'kd':kd})

    fsbb.boost_energy.reset()

def _config_boost_energy_mpc_controller(fsbb, model_params, ctl_params):
    
    f_pwm = model_params['f_pwm']

    C = model_params['C_out']
    L = model_params['L']
    dt = 1 / f_pwm

    fsbb.boost_energy_mpc.set_params({'C':C, 'L':L, 'dt':dt})

    il_lim = ctl_params['il_lim']
    filt_coef = ctl_params['filt_coef']
    filt_en = ctl_params['filt_en']
    kd = ctl_params['kd']
    fsbb.boost_energy_mpc.set_params(
        {'il_lim':il_lim, 'filt_coef':filt_coef, 'filt_en':filt_en, 'kd':kd}
        )
    
    alpha = ctl_params['alpha']
    rw = ctl_params['rw']
    l_pred = ctl_params['l_pred']
    fsbb.boost_energy_mpc.set_gains(rw=rw, l_pred=l_pred, alpha=alpha, dt=dt)    

    fsbb.boost_energy_mpc.reset()
