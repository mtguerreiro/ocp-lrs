"""
Module ``buck_controller``
===========================


"""
import time

import pyocp
import struct
import numpy as np
import scipy.signal
import control
import pyctl

from dataclasses import dataclass

@dataclass
class ModelParams:
    v_in : float = 16
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

        self.idle = _Idle(0, ctl_if)
        self.ramp = _Ramp(1, ctl_if)
        self.sfb = _SFB(2, ctl_if)


class _Idle(pyocp.controller.ControllerTemplate):
    
    def __init__(self, ctl_id, ctl_if):
        super().__init__(ctl_id, ctl_if)


class _Ramp(pyocp.controller.ControllerTemplate):
    
    def __init__(self, ctl_id, ctl_if):
        super().__init__(ctl_id, ctl_if)

        self.keys = ('u_step', 'u_ref')
        

    def _decode(self, params_bin):
        
        return _dict_decode(self.keys, params_bin)


    def _encode(self, params):

        return _dict_encode(self.keys, params)


class _SFB(pyocp.controller.ControllerTemplate):
    
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
        
        return _dict_decode(self.keys, params_bin)


    def _encode(self, params):

        return _dict_encode(self.keys, params)


def _dict_encode(keys, params):
    
    _params = [params[key] for key in keys]
    params_bin = struct.pack(f'<{len(keys)}f', *_params)

    time.sleep(0.0025) # because of c2000

    return params_bin    


def _dict_decode(keys, params_bin):
    
    _params = struct.unpack(f'<{len(keys)}f', params_bin)
    params = dict(zip(keys, _params))

    return params
