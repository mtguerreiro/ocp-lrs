
def config_controllers(fsbb, model_params, plat_params):

    if 'energy_params' in plat_params:
        _config_energy_controller(
            fsbb,
            model_params,
            plat_params['energy_params']
        )

    if 'energy_mpc_params' in plat_params:
        _config_energy_mpc_controller(
            fsbb,
            model_params,
            plat_params['energy_mpc_params']
        )


def _config_energy_controller(fsbb, model_params, ctl_params):
    
    f_pwm = model_params['f_pwm']

    ts = ctl_params['ts']
    os = ctl_params['os']
    dt = 1 / f_pwm
    fsbb.boost_energy.set_gains(ts=ts, os=os, dt=dt)

    alpha = ctl_params['alpha']
    filt_en = ctl_params['filt_en']
    kd = ctl_params['kd']
    fsbb.boost_energy.set_params({'alpha':alpha, 'filt_en':filt_en, 'kd':kd})


def _config_energy_mpc_controller(fsbb, model_params, ctl_params):
    
    f_pwm = model_params['f_pwm']

    C = model_params['C_out']
    L = model_params['L']
    dt = 1 / f_pwm

    fsbb.boost_energy_mpc.set_params({'C':C, 'L':L, 'dt':dt})

    il_lim = ctl_params['il_lim']
    filt_coef = ctl_params['filt_coef']
    filt_en = ctl_params['filt_en']
    kd = ctl_params['kd']
    print(f'setting il_lim to {il_lim}')
    fsbb.boost_energy_mpc.set_params(
        {'il_lim':il_lim, 'filt_coef':filt_coef, 'filt_en':filt_en, 'kd':kd}
        )
    
    alpha = ctl_params['alpha']
    rw = ctl_params['rw']
    l_pred = ctl_params['l_pred']
    fsbb.boost_energy_mpc.set_gains(rw=rw, l_pred=l_pred, alpha=alpha, dt=dt)    
