import struct
import time

import numpy as np

import pyocp
from .controllers import Controllers, Reference
from .trace import Trace
from .hw import Hw

class Interface(Controllers, Reference):
    
    def __init__(self, comm_type, settings, cs_id=0, tr_id=0):
        self._ocp = pyocp.ocp.Interface(comm_type='ethernet', settings=settings)
        self._cs_id = cs_id
        self._tr_id = tr_id

        self._ctl_if = pyocp.controller.Interface(self._ocp.cs_controller_if, cs_id)

        Controllers.__init__(self, self._ctl_if)
        Reference.__init__(self, self._ctl_if)

        self.trace = Trace(tr_id=self._tr_id, ocp_if=self._ocp)

        self.hw = Hw(self._ocp, cs_id)


    def enable(self):

        return self._ocp.cs_enable(self._cs_id)


    def disable(self):

        return self._ocp.cs_disable(self._cs_id)

    
    def init_buck_plecs_controller(self, vo_ref=0):

        self.disable()                   ; time.sleep(0.015)
        self.hw.clear_status()           ; time.sleep(0.015)
        self.hw.set_input_relay(1)       ; time.sleep(0.015)
        self.hw.set_output_relay(1)      ; time.sleep(0.015)
        time.sleep(0.2)

        if( self._run_enable_procedure() != 0 ): return -1
        
        self.set_ref(vo_ref)             ; time.sleep(0.015)
        self.plecs.reset()               ; time.sleep(0.015)
        self.plecs.enable()              ; time.sleep(0.015)

        return 0


    def set_output_voltage_trigger(self, voltage, size=1000, pre_trig=100):

        return self._set_trace_trigger(7, voltage, size=size, pre_trig=pre_trig)


    def set_output_load_trigger(self, size=1000, pre_trig=100):

        return self._set_trace_trigger(6, 0.5, size=size, pre_trig=pre_trig)
    

    def get_transient_data(self):

        status, data = self.trace.read()             ; time.sleep(0.015)
        if status != 0:
            print('Error reading trace data')
            return (-1, status)

        f_pwm = 100e3 # TODO: get it from the hardware

        t = 1 / f_pwm * np.arange(data.shape[0]) / 1e-3

        return t, data


    def shutdown(self):

        self.ramp.set_params({'u_step': 0.001, 'u_ref': 0.0}) ; time.sleep(0.015)
        self.ramp.enable()                                    ; time.sleep(0.015)
        time.sleep(0.5)
        self.disable()                                        ; time.sleep(0.015)
        self.hw.set_output_relay(0)                           ; time.sleep(0.015)
        self.hw.set_input_relay(0)                            ; time.sleep(0.015)

    
    def _run_enable_procedure(self):
        
        # Procedure to enable the controller. If the hardware is run for the first
        # time, the adc will give an invalid measurement that will trigger an error.
        # This procedure enables/disables/enables the controller as a work-around.
        self.idle.enable()                         ; time.sleep(0.015)   

        self.enable()                              ; time.sleep(0.015)
        self.disable()                             ; time.sleep(0.015)
        self.hw.clear_status()                     ; time.sleep(0.015)

        self.idle.enable()                         ; time.sleep(0.015)
        self.enable()                              ; time.sleep(0.015)
        time.sleep(0.1)
        status, hw_status = self.hw.get_status()   ; time.sleep(0.015)
        if hw_status != 0:
            self.disable()
            print('Failed to enable the controller...')
            return -1

        return 0


    def _set_trace_trigger(self, idx, level, size=1000, pre_trig=100):
        
        self.trace.set_n_pre_trig_samples(int(pre_trig))   ; time.sleep(0.015)
        self.trace.set_size(int(size))                     ; time.sleep(0.015)

        self.trace.set_trig_level(float(level))            ; time.sleep(0.015)
        self.trace.set_trig_signal(int(idx))               ; time.sleep(0.015)

        self.trace.set_mode(1)                             ; time.sleep(0.015)

        self.trace.reset()                                 ; time.sleep(0.015)

        return 0
