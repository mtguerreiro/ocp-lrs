"""
Module ``controllers``
======================


"""
import pyocp
import struct
import numpy as np
import scipy.signal
import control
import pyctl

from dataclasses import dataclass

@dataclass
class ModelParams:
    v_in : float = 30
    R : float = 4
    L : float = 15e-6
    Co : float = 100e-6


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

        self.idle = _Idle(0, ctl_if)
        self.ramp = _Ramp(1, ctl_if)
        self.cpl = _CPL(2, ctl_if)
        self.buck_sfb = _BuckSFB(3, ctl_if)
        self.buck_plecs = _BuckPlecs(4, ctl_if)
        self.boost_energy = _BoostEnergy(5, ctl_if)
        self.boost_energy_mpc = _BoostEnergyMpc(6, ctl_if)


class _Idle(pyocp.controller.ControllerTemplate):
    
    def __init__(self, ctl_id, ctl_if):
        super().__init__(ctl_id, ctl_if)


class _Ramp(pyocp.controller.ControllerTemplate):
    
    def __init__(self, ctl_id, ctl_if):
        super().__init__(ctl_id, ctl_if)

        self.keys = ('u_step', 'u_ref')
        

    def _decode(self, params_bin):
        
        keys = self.keys
        
        _params = struct.unpack(f'<{len(keys)}f', params_bin)
        params = dict(zip(keys, _params))

        return params


    def _encode(self, params):

        keys = self.keys
        
        _params = [params[key] for key in keys]
        params_bin = struct.pack(f'<{len(keys)}f', *_params)

        return params_bin


class _CPL(pyocp.controller.ControllerTemplate):

    def __init__(self, ctl_id, ctl_if):
        super().__init__(ctl_id, ctl_if)

        self.keys = ('ki', 'kv', 'k_ev', 'dt', 'v_min', 'v_max')
        self._model_params = ModelParams


    def get_model_params(self):

        return self._model_params


    def set_model_params(self, params):

        self._model_params = params


    def set_gains(self, ts=1e-3, os=5, dt=1/100e3):

        params = self._get_gains(ts=ts, os=os, dt=dt)

        return self.set_params(params)


    def _get_gains(self, ts=1e-3, os=5, dt=1/100e3):

        # Model
        V_in = self._model_params.v_in
        R = self._model_params.R
        L = self._model_params.L
        Co = self._model_params.Co

        ki, kv, k_ev = pyctl.design.pe.buck.sfb(
            V_in, R, L, Co,
            ts, os, alpha=5
        )
        
        return {'ki':ki, 'kv':kv, 'k_ev':k_ev, 'dt':dt}


    def _decode(self, params_bin):

        keys = self.keys

        _params = struct.unpack(f'<{len(keys)}f', params_bin)
        params = dict(zip(keys, _params))

        return params


    def _encode(self, params):

        keys = self.keys

        _params = [params[key] for key in keys]
        params_bin = struct.pack(f'<{len(keys)}f', *_params)

        return params_bin


class _BuckSFB(pyocp.controller.ControllerTemplate):
    
    def __init__(self, ctl_id, ctl_if):
        super().__init__(ctl_id, ctl_if)

        self.keys = ('ki', 'kv', 'k_ev', 'dt')
        self._model_params = ModelParams


    def get_model_params(self):

        return self._model_params
    

    def set_model_params(self, params):

        self._model_params = params

        
    def set_gains(self, ts=1e-3, os=5, dt=1/100e3):

        params = self._get_gains(ts=ts, os=os, dt=dt)

        return self.set_params(params)        


    def _get_gains(self, ts=1e-3, os=5, dt=1/100e3):

        # Model
        V_in = self._model_params.v_in
        R = self._model_params.R
        L = self._model_params.L
        Co = self._model_params.Co
        
        ki, kv, k_ev = pyctl.design.pe.buck.sfb(
            V_in, R, L, Co,
            ts, os, alpha=5
        )
        
        return {'ki':ki, 'kv':kv, 'k_ev':k_ev, 'dt':dt}


    def _decode(self, params_bin):
        
        keys = self.keys
        
        _params = struct.unpack(f'<{len(keys)}f', params_bin)
        params = dict(zip(keys, _params))

        return params


    def _encode(self, params):

        keys = self.keys
        
        _params = [params[key] for key in keys]
        params_bin = struct.pack(f'<{len(keys)}f', *_params)

        return params_bin


class _BuckPlecs(pyocp.controller.ControllerTemplate):
    
    def __init__(self, ctl_id, ctl_if):
        super().__init__(ctl_id, ctl_if)

        self.keys = ()


class _BoostEnergy(pyocp.controller.ControllerTemplate):
    
    def __init__(self, ctl_id, ctl_if):
        super().__init__(ctl_id, ctl_if)

        self.keys = (
            'k1', 'k2', 'k3', 'dt',
            'C', 'L', 'alpha', 'filt_en',
            'kd'
            )
        self._model_params = ModelParams
        

    def _decode(self, params_bin):
        
        keys = self.keys
        
        _params = struct.unpack(f'<{len(keys)}f', params_bin)
        params = dict(zip(keys, _params))

        return params


    def _encode(self, params):

        keys = self.keys
        
        _params = [params[key] for key in keys]
        params_bin = struct.pack(f'<{len(keys)}f', *_params)

        return params_bin
    

    def set_gains(self, ts=2e-3, os=5, dt=1/100e3):

        params = self._get_gains(ts, os=os, dt=dt)
        
        return self.set_params(params)


    def _get_gains(self, ts, os=5, alpha=5.0, dt=1.0):

        k1, k2, k3 = pyctl.design.pe.boost.energy(ts, os, alpha=alpha)

        gains = {'k1':k1, 'k2':k2, 'k3':k3, 'dt':dt}

        return gains


class _BoostEnergyMpc(pyocp.controller.ControllerTemplate):
    
    def __init__(self, ctl_id, ctl_if):
        super().__init__(ctl_id, ctl_if)

        self.keys = (
            'C', 'L', 'ts', 'il_lim', 'alpha',
            'Ky', 'K_dz_1', 'K_dz_2',
            'filt_coef', 'filt_en',
            'kd'
            )
        self._model_params = ModelParams
        

    def _decode(self, params_bin):
        
        keys = self.keys
        
        _params = struct.unpack(f'<{len(keys)}f', params_bin)
        params = dict(zip(keys, _params))

        return params


    def _encode(self, params):

        keys = self.keys
        
        _params = [params[key] for key in keys]
        params_bin = struct.pack(f'<{len(keys)}f', *_params)

        return params_bin
    

    def set_gains(self, rw=1e-5, l_pred=12, alpha=1e6, dt=1/100e3):

        params = self._get_gains(rw=rw, l_pred=l_pred, alpha=alpha, dt=dt)
        
        return self.set_params(params)


    def _get_gains(self, rw=1e-5, l_pred=12, alpha=1e6, dt=1.0):

        Am = np.array([[0.0, 1.0],
                       [0.0, 0.0]])

        Bm = np.array([[0.0],
                       [alpha]])

        Cm = np.array([1.0, 0.0])
        
        Ad, Bd, Cd, _, _ = scipy.signal.cont2discrete((Am, Bm, Cm, 0), dt, method='zoh')

        sys = pyctl.mpc.System(Ad, Bd, Cd, l_pred=l_pred, rw=rw)

        gains = {
            'Ky':sys.Ky[0][0],
            'K_dz_1':sys.Kx[0][0], 'K_dz_2':sys.Kx[0][1],
            'dt':dt, 'alpha':alpha
        }

        return gains
