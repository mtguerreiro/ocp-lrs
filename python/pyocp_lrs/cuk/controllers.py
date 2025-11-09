"""
Module ``controllers``
======================


"""
import pyocp
import struct
import numpy as np
import scipy.signal
import pyctl

from dataclasses import dataclass

@dataclass
class ModelParams:
    Vi : float = 25
    Vo : float = 25
    Po : float = 120
    
    L1 : float = 100e-6
    L2 : float = 150e-6
    Cc : float = 9.4e-6
    Co : float = 330e-6

    N : float = 5 / 3

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
        self.energy = _Energy(3, ctl_if)
        self.energy_mpc = _EnergyMpc(4, ctl_if)


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


class _Energy(pyocp.controller.ControllerTemplate):
    
    def __init__(self, ctl_id, ctl_if):
        super().__init__(ctl_id, ctl_if)

        self.keys = (
            'k1', 'k2', 'k3', 'dt',
            'a0', 'a1', 'a2', 'b1', 'b2', 'notch_en',
            'C_out',
        )
        

    def _decode(self, params_bin):
        
        keys = self.keys
        
        _params = struct.unpack(f'<{len(keys)}f', params_bin)
        params = dict(zip(keys, _params))

        return params
    

    def get_model_params(self):

        return self._model_params
    

    def set_model_params(self, params):

        self._model_params = params
        

    def _encode(self, params):

        keys = self.keys
        
        _params = [params[key] for key in keys]
        params_bin = struct.pack(f'<{len(keys)}f', *_params)

        return params_bin


    def set_notch(self, fc, Q=0.5, dt=1/100e3, enable=False):

        params = self._discrete_notch(fc, Q=Q, dt=dt)

        if enable is False:
            params['notch_en'] = 0
        else:
            params['notch_en'] = 1
        
        return self.set_params(params)
    
        
    def set_gains(self, ts=3e-3, os=5, dt=1/100e3):

        params = self._get_gains(ts=ts, os=os, dt=dt)

        return self.set_params(params)


    def _get_gains(self, ts=1e-3, os=5, dt=1/100e3, method='approx', alpha=5):

        # Poles
        if method == 'approx':
            zeta = -np.log(os/100) / np.sqrt(np.pi**2 + (np.log(os/100))**2)
            wn = 4/ts/zeta

            p1 = -zeta * wn + wn * np.sqrt(zeta**2 - 1, dtype=complex)
            p2 = np.conj(p1)
            p3 = alpha * p1.real

            poles = [p1, p2, p3]

        elif method == 'bessel':
            p1 = (-3.9668 + 3.7845j) / ts
            p2 = np.conj(p1)
            p3 = -5.0093 / ts

        elif method == 'itae':
            p1 = (-4.35 + 8.918j) / ts
            p2 = np.conj(p1)
            p3 = -5.913 / ts

        else:
            print('Unknown method')
            return 0
            
        poles = [p1, p2, p3]
        
        # Augmented model        
        A = np.array([[ 0.0, 1.0, 0.0],
                      [ 0.0, 0.0, 0.0],
                      [-1.0, 0.0, 0.0]])

        B = np.array([[0.0], [1.0], [0.0]])

        # Gains
        K = scipy.signal.place_poles(A, B, poles).gain_matrix.reshape(-1)

        gains = {'k1':K[0], 'k2':K[1], 'k3':K[2], 'dt':dt}

        return gains


    def _discrete_notch(self, fc, Q, dt):
        
        wc = 2 * np.pi * fc
        
        num = [1, 0 , wc**2]
        den = [1, wc/Q, wc**2]
        tf = (num, den)

        num_d, den_d, _ = scipy.signal.cont2discrete(tf, dt)
        num_d = num_d.reshape(-1) / den_d[0]

        filt = {'a0':num_d[0], 'a1':num_d[1], 'a2':num_d[2], 'b1':den_d[1], 'b2':den_d[2]}
        
        return filt



class _EnergyMpc(pyocp.controller.ControllerTemplate):
    
    def __init__(self, ctl_id, ctl_if):
        super().__init__(ctl_id, ctl_if)

        self.keys = [
            'Ky', 'K_dz_1', 'K_dz_2', 'dt',
            'alpha', 'Co', 'il_max', 'il_min',
            'Ku_freq_1'
        ]

        for i in range(200):
            self.keys.append(f'Ku_freq_2_{i}')
        

    def _decode(self, params_bin):
        
        keys = self.keys

        _params = struct.unpack(f'<{len(keys)}f', params_bin)
        params = dict(zip(keys, _params))

        return params
    

    def get_model_params(self):

        return self._model_params
    

    def set_model_params(self, params):

        self._model_params = params
        

    def _encode(self, params):

        keys = self.keys
        
        _params = [params[key] for key in keys]
        params_bin = struct.pack(f'<{len(keys)}f', *_params)

        return params_bin
    
        
    def set_gains(
        self,
        rw=0.05, l_pred=200, alpha=1e6,
        fw=None, qw=None, l_past=None, window='hann',
        il_max=6, il_min=0.15, Co=330e-6,
        dt=1/100e3
        ):

        if l_past is not None:
            qw = self._get_q(fw, qw, dt, l_pred, l_past)

        mpc_params = self._get_mpc_gains(
            rw=rw, l_pred=l_pred, alpha=alpha,
            qw=qw, l_past=l_past, window=window,
            dt=dt
        )

        params = dict(mpc_params)
        params['Co'] = Co
        params['il_max'] = il_max
        params['il_min'] = il_min

        return self.set_params(params)


    def _get_mpc_gains(
        self,
        rw=0.05, l_pred=200, alpha=1e6,
        qw=None, l_past=None, window='hann',
        dt=1/100e3
        ):

        Am = np.array([[0.0, 1.0],
               [0.0, 0.0]])

        Bm = np.array([[0.0],
                       [alpha]])

        Cm = np.array([1.0, 0.0])
        
        Ad, Bd, Cd, _, _ = scipy.signal.cont2discrete((Am, Bm, Cm, 0), dt, method='zoh')

        sys = pyctl.mpc.System(
            Ad, Bd, Cd,
            l_pred=l_pred, rw=rw,
            l_past=l_past, q=qw, window=window
        )

        #sys.export(r'/home/marco/projects/ocp-lrs/lrs_apps/cuk/cdmpc/')

        Ky = sys.Ky[0][0]
        K_dz_1 = sys.Kx[0][0]
        K_dz_2 = sys.Kx[0][1]
        Ku_freq = sys.Ku_freq[0]
        Ku_freq_1 = np.sum(Ku_freq[l_past:])
        Ku_freq_2 = Ku_freq[:l_past]

        _gains = {
            'Ky':Ky, 'K_dz_1':K_dz_1, 'K_dz_2':K_dz_2,
            'Ku_freq_1': Ku_freq_1, 'Ku_freq_2': Ku_freq_2,
            'dt':dt, 'alpha':alpha
        }
        
        for i in range(l_past):
            _gains[f'Ku_freq_2_{i}'] = Ku_freq_2[i].astype(np.float32)#.tobytes()

        return _gains


    def _get_q(self, f, qw, dt, l_pred, l_past):

        l = l_pred + l_past
        df = (1 / dt) / l

        idx = [round(fi / df) for fi in f]

        q = np.zeros(l)
        for i, qwi in zip(idx, qw):
            q[i] = qwi
            if i != 0: q[l - i] = qwi

        return q
