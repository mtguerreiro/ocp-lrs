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


    def init_buck_plecs_controller(self, vo_ref=0):

        self.disable()
        self.hw.clear_status()
        self.hw.set_load_switch(0)
        #self.hw.set_input_relay(1)
        self.hw.set_output_relay(1)
        self.set_converter_mode('buck')
        time.sleep(0.2)

        if( self._run_enable_procedure() != 0 ): return -1
        
        self.set_ref(vo_ref)
        self.plecs.reset()
        self.plecs.enable()

        return 0

    
    def init_boost_plecs_controller(self, vo_ref=0):

        self.disable()
        self.hw.clear_status()
        self.hw.set_load_switch(0)
        #self.hw.set_input_relay(1)
        self.hw.set_output_relay(1)
        self.set_converter_mode('boost')

        if( self._run_enable_procedure() != 0 ): return -1
        
        self.set_ref(vo_ref)
        self.plecs.reset()
        self.plecs.enable()

        return 0


    def set_output_voltage_trigger(self, voltage, size=1000, pre_trig=100):

        return self._set_trace_trigger(b'Voltage reference', voltage, size=size, pre_trig=pre_trig)


    def set_output_load_trigger(self, size=1000, pre_trig=100):

        return self._set_trace_trigger(b'Load switch', 0.5, size=size, pre_trig=pre_trig)


    def _set_trace_trigger(self, signal_label, level, size=1000, pre_trig=100):

        status, traces = self.trace.get_signals()
        if( status != 0 ):
            print('Failed to set trigger: unable to read the trace signals.')
            return -1

        try:
            idx = traces.index(signal_label)
        except:
            print('Failed to set trigger: voltage reference not in signal list.')
            return -1
        
        self.trace.set_n_pre_trig_samples(int(pre_trig))
        self.trace.set_size(int(size))

        self.trace.set_trig_level(float(level))
        self.trace.set_trig_signal(idx)

        self.trace.set_mode(1)

        self.trace.reset()

        return 0
        
    
    def get_transient_data(self):

        status, data = self.trace.read()
        if status != 0:
            print('Error reading trace data')
            return (-1, status)

        status, f_pwm = self.hw.get_pwm_frequency()
        if status != 0:
            print('Error reading trace data')
            return (-1, status)

        t = 1 / f_pwm * np.arange(data.shape[0]) / 1e-3

        return t, data

    
    def shutdown(self):

        self.ramp.set_params({'u_step': 0.001, 'u_ref': 0.0})
        self.ramp.enable()
        time.sleep(0.5)
        self.disable()
        self.hw.set_load_switch(0)
        self.hw.set_output_relay(0)
        self.hw.set_input_relay(0)


    def _run_enable_procedure(self):
        
        # Procedure to enable the controller. If the hardware is run for the first
        # time, the adc will give an invalid measurement that will trigger an error.
        # This procedure enables/disables/enables the controller as a work-around.
        self.idle.enable()

        self.enable()
        self.disable()
        self.hw.clear_status()

        self.idle.enable()
        self.enable()
        time.sleep(0.1)
        status, hw_status = self.hw.get_status()
        if hw_status != 0:
            self.disable()
            print('Failed to enable the controller...')
            return -1

        return 0


    def enable(self):

        return self._ocp.cs_enable(self._cs_id)


    def disable(self):

        return self._ocp.cs_disable(self._cs_id)


    def set_converter_mode(self, mode):

        modes = ('buck', 'boost')
        if mode not in modes:
            raise ValueError(f'Mode must be one of: {modes}')

        status, en = self.hw.get_pwm_output_enable()
        if status != 0:
            return (-1, status)

        if en != 0:
            print('Cannot set mode if pwm is enabled.')
            return (-1,)

        status = self.hw.set_pwm_ls_sw(0)
        if status[0] != 0:
            print ('Failed to set the low-side switch.')
            return (-1, status[0])

        status = self.hw.set_pwm_hs_sw(1)
        if status[0] != 0:
            print ('Failed to set the high-side switch.')
            return (-1, status[0])

        if mode == 'buck':
            status = self.hw.set_pwm_mode(0)
        elif mode == 'boost':
            status = self.hw.set_pwm_mode(1)
        else:
            print('Unknown mode')
            return (-1,)

        if status[0] != 0:
            print('Failed to set the pwm mode')
            return (-1, status[0])

        return (0,)
