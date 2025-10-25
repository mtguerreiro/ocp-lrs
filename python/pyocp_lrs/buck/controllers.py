"""
Module ``buck_controller``
===========================


"""
import pyocp
import struct
import numpy as np
import scipy.signal

from dataclasses import dataclass

@dataclass
class _ModelParams:
    V_in : float = 16
    R : float = 20
    L : float = 47e-6
    Co : float = 150e-6


class Reference(pyocp.controller.ReferenceTemplate):

    def __init__(self, ctl_if):
        super().__init__(ctl_if)


    def _decode(self, ref_bin):
        
        fmt = '<f'
        ref = struct.unpack(fmt, ref_bin)

        return ref


    def _encode(self, ref):

        fmt = '<f'
        
        ref_bin = struct.pack(fmt, ref)

        return ref_bin


class Controllers:

    def __init__(self, ctl_if):

        self.disabled = _Disabled(0, ctl_if)
        self.startup = _Startup(1, ctl_if)
        self.sfb = _SFB(2, ctl_if)
        #self.cascaded = _Cascaded(3, ctl_if)


class _Disabled(pyocp.controller.ControllerTemplate):
    
    def __init__(self, ctl_id, ctl_if):
        super().__init__(ctl_id, ctl_if)


class _Startup(pyocp.controller.ControllerTemplate):
    
    def __init__(self, ctl_id, ctl_if):
        super().__init__(ctl_id, ctl_if)
        

    def _decode(self, params_bin):
        
        fmt = '<' + 'f' * round( len(params_bin) / 4 )
        data = struct.unpack(fmt, params_bin)

        params = {}
        params['uinc'] = data[0]
        params['ufinal'] = data[1]

        return params


    def _encode(self, params):

        fmt = '<' + 'f' * len(params)
        
        uinc = params['uinc']
        ufinal = params['ufinal']

        params_bin = struct.pack(fmt, uinc, ufinal)

        return params_bin


class _SFB(pyocp.controller.ControllerTemplate):
    
    def __init__(self, ctl_id, ctl_if):
        
        super().__init__(ctl_id, ctl_if)
        self._model_params = _ModelParams


    def _decode(self, params_bin):
        
        fmt = '<' + 'f' * round( len(params_bin) / 4 )
        data = struct.unpack(fmt, params_bin)

        params = {}
        params['k1'] = data[0]
        params['k2'] = data[1]
        params['ke'] = data[2]
        params['dt'] = data[3]

        return params


    def _encode(self, params):

        fmt = '<' + 'f' * len(params)
        
        k1 = params['k1']
        k2 = params['k2']
        ke = params['ke']
        dt = params['dt']
        
        params_bin = struct.pack(fmt, k1, k2, ke, dt)

        return params_bin


    def get_model_params(self):

        return self._model_params
    

    def set_model_params(self, params):

        self._model_params = params


    def set_gains(self, ts=1e-3, os=5):

        params = self._get_gains(ts=ts, os=os)

        return self.set_params(params)


    def _get_gains(self, ts=1e-3, os=5):

        # Model
        V_in = self._model_params.V_in
        R = self._model_params.R
        L = self._model_params.L
        Co = self._model_params.Co
        
        A = np.array([
            [0,         -1 / L],
            [1 / Co,    -1 / R / Co]
            ])
        
        B = np.array([
            [V_in / L],
            [0]
            ])

        C = np.array([0, 1])

        # Augmented model (integrator)
        Aa = np.zeros((3,3))
        Aa[:2, :2] = A
        Aa[2, :2] = -C

        Ba = np.zeros((3,1))
        Ba[:2, 0] = B[:, 0]

        # Poles
        zeta = -np.log(os/100) / np.sqrt( np.pi**2 + (np.log(os/100))**2 )
        wn = 4 / ts / zeta

        p1 = -zeta * wn + 1j * wn * np.sqrt(1 - zeta**2)
        p2 = p1.conj()
        p3 = 5 * p1.real
        p = [p1, p2, p3]
        
        # Controller design
        K = scipy.signal.place_poles(Aa, Ba, p).gain_matrix[0]
        
        ki = K[0]
        kv = K[1]
        k_ev = K[2]
        
        return {'k1':ki, 'k2':kv, 'ke':k_ev}


class _Cascaded(pyocp.controller.ControllerTemplate):

    def __init__(self, ctl_id, ctl_if):

        super().__init__(ctl_id, ctl_if)
        self._model_params = _ModelParams


    def _decode(self, params_bin):
        
        fmt = '<' + 'f' * round( len(params_bin) / 4 )
        data = struct.unpack(fmt, params_bin)

        params = {}
        params['ki'] = data[0]
        params['k_ei'] = data[1]
        params['kv'] = data[2]
        params['k_ev'] = data[3]

        params['i_max'] = data[4]
        params['i_min'] = data[5]

        return params


    def _encode(self, params):

        fmt = '<' + 'f' * len(params)
        
        ki = params['ki']
        k_ei = params['k_ei']
        kv = params['kv']
        k_ev = params['k_ev']

        i_max = params['i_max']
        i_min = params['i_min']

        params_bin = struct.pack(fmt, ki, k_ei, kv, k_ev, i_max, i_min)

        return params_bin


    def get_model_params(self):

        return self._model_params
        

    def set_model_params(self, params):

        self._model_params = params


    def set_gains(self, ts=1e-3, os=5):

        params = self._get_gains(ts=ts, os=os)

        return self.set_params(params)


    def _get_gains(self, ts=1e-3, os=5):

        # Model
        V_in = self._model_params.V_in
        R = self._model_params.R
        L = self._model_params.L
        CO = self._model_params.Co
        
        ts_v = ts
        os_v = os
        
        os_i = os_v
        ts_i = ts_v / 5

        zeta_i, wn_i = self._zeta_wn(ts_i, os_i)
        ki = (L / V_in) * 2 * zeta_i * wn_i
        k_ei = (L / V_in) * ( - wn_i**2 )

        zeta_v, wn_v = self._zeta_wn(ts_v, os_v)
        kv = ( CO ) * ( 2 * zeta_v * wn_v - 1 / R / CO )
        k_ev = ( CO ) * ( - wn_v**2 )

        return {'ki': ki, 'k_ei': k_ei, 'kv': kv, 'k_ev':k_ev}


    def _zeta_wn(self, ts, os):

        zeta = -np.log(os / 100) / np.sqrt( np.pi**2 + np.log(os / 100)**2 )
        wn = 4 / ts / zeta

        return (zeta, wn)

##class Controllers:
##    """
##    """
##    def __init__(self):
##        self.ctl = {
##            0            : {'id':0, 'if':None},
##            'startup'    : {'id':1, 'if':Startup()},
##            'sfb_int'    : {'id':2, 'if':SfbInt()},
##            }


##class Startup:
##    def __init__(self):
##        pass
##    
##
##    def set(self, params):
##
##        uinc = params['uinc']
##        ufinal = params['ufinal']
##        data = list(struct.pack('<ff', uinc, ufinal))
##        
##        return data
##    
##
##    def get(self, data):
##
##        pars = struct.unpack('<ff', data)
##
##        params = {
##            'uinc': pars[0],
##            'ufinal': pars[1]
##            }
##
##        return params
##
##
##class SfbInt:
##    def __init__(self, model_params={}):
##
##        self._params = {}
##
##        self._params['V_in'] = 25
##
##        self._params['L'] = 47e-6
##        self._params['Co'] = 150e-6
##
##        self._params['R_load'] = 3.3
##
##        for p, v in model_params.items():
##            if p in self._params:
##                self._params[p] = v
##
##
##    def set(self, params):
##
##        k1 = params['k1']
##        k2 = params['k2']
##        ke = params['ke']
##        dt = params['dt']
##                
##        data = list(struct.pack('<ffff', k1, k2, ke, dt))
##        
##        return data
##    
##
##    def get(self, data):
##
##        pars = struct.unpack('<ffff', data)
##
##        params = {
##            'k1': pars[0],
##            'k2': pars[1],
##            'ke': pars[2],
##            'dt': pars[3],
##            }
##
##        return params
##    
##
##    def params(self, ts, os=5, method='approx', dt=1.0, model_params={}):
##
##        for p, v in model_params.items():
##            if p in self.params:
##                self.params[p] = v
##                
##        V_in = self._params['V_in']
##
##        L = self._params['L']
##        Co = self._params['Co']
##        R_load = self._params['R_load']
##
##        a11 = 0; a12 = -1 / L;
##        a21 = 1 / Co; a22 = - 1 / R_load / Co
##        A = np.array([[a11, a12],
##                      [a21, a22]])
##
##        b11 = V_in / L; b21 = 0
##
##        B = np.array([ [b11], [b21] ])
##
##        C = np.array([0, 1])
##
##        # Augmented model
##        n_st = A.shape[0]
##        Aa = np.zeros((n_st+1, n_st+1))
##        Aa[:n_st, :n_st] = A
##        Aa[-1, :n_st] = -C
##
##        Ba = np.zeros((n_st + 1, 1))
##        Ba[:n_st, 0] = B[:, 0]
##
##        # Poles
##        if method == 'approx':
##            zeta = -np.log(os/100) / np.sqrt( np.pi**2 + (np.log(os/100))**2 )
##            wn = 4 / zeta / ts
##            
##            p1 = -zeta*wn + wn * np.emath.sqrt(zeta**2 - 1)
##            p2 = -zeta*wn - wn * np.emath.sqrt(zeta**2 - 1)
##
##            p3 = 5*p1.real
##
##        #elif method == 'bessel':
##        #    p1 = (-4.1104 + 6.3142j) / ts
##        #    p2 = np.conj(p1)
##        #    p3 = (-5.9268 + 3.0813j) / ts
##
##        #elif method == 'itae':
##        #    p1 = (-3.948 + 13.553j) / ts
##        #    p2 = np.conj(p1)
##        #    p3 = (-6.040 + 5.601j) / ts
##
##        else:
##            print('Unknown method')
##            return 0
##
##        poles = [p1, p2, p3]
##
##        sys = control.ss(A, B, C, 0)
##        #print('Pole placement.\nMethod: {:}'.format(method))
##        #print('Poles: {:}'.format(poles))
##        #print('Zeros: {:}'.format(sys.zeros()))
##        #print('Open-loop poles: {:}'.format(sys.poles()))
##        
##        # State feedback
##        K = scipy.signal.place_poles(Aa, Ba, poles).gain_matrix[0]
##
##        ctl_params = {'k1':K[0], 'k2':K[1], 'ke':K[2], 'dt':dt}
##
##        return ctl_params
##
    

