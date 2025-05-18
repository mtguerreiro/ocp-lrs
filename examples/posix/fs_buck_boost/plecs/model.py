def params():

    V_in = 8

    Rs = 50e-3
    Ls = 100e-9

    L = 15e-6
    RL = 15e-3

    C_in = 100e-6
    R_C_in = 25e-3
    V_in_ini = V_in
    R_pc = 3.9e3
    
    C_out = 100e-6
    R_C_out = 25e-3
    V_out_ini = 0

    R_load = 5
    
    f_pwm = 100e3
    R_ds = 25e-3

    _params = {}

    _params['V_in'] = V_in

    _params['Rs'] = Rs
    _params['Ls'] = Ls
    
    _params['L'] = L
    _params['RL'] = RL
    
    _params['C_in'] = C_in
    _params['R_C_in'] = R_C_in
    _params['V_in_ini'] = V_in_ini
    _params['R_pc'] = R_pc
    
    _params['C_out'] = C_out
    _params['R_C_out'] = R_C_out
    _params['V_out_ini'] = V_out_ini
        
    _params['f_pwm'] = f_pwm
    _params['R_ds'] = R_ds

    _params['R_load'] = R_load
    
    return _params
