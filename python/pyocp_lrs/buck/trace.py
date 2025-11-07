import struct
import numpy as np

import pyocp
import time

"""
Trace functions that have more than one OCP command are reimplemented here
in order to add some delay between subsequent OCP calls. This is to handle
the C2000 + W5500 setup.
"""
class Trace(pyocp.trace.TraceTemplate):

    def __init__(self, ocp_if, tr_id=0):

        super().__init__(tr_id=tr_id, ocp_if=ocp_if)

    
    def read(self):

        status, data_bin = self._ocp_if.trace_read(self._tr_id)
        if status < 0:
            return (-1, status)
        time.sleep(0.05)

        status, mode = self.get_mode()
        if status < 0:
            return (-1, status)
        time.sleep(0.05)
        
        if mode == self._TRACE_MODE_TRIGGER:
            status, data_bin = self._reorder_data(data_bin)
            if status < 0:
                return (-1, status)
        time.sleep(0.05)

        status, data = self._decode(data_bin)
        if status < 0:
            return(-1, status)
        
        return (status, data)


    def get_size(self):

        status, tr_size = self._ocp_if.trace_get_size(self._tr_id)
        if status < 0:
            return (-1, status)
        time.sleep(0.025)
        
        status, n_signals = self._ocp_if.trace_get_number_signals(self._tr_id)
        if status < 0:
            return (-1, status)

        size = tr_size / n_signals

        return (0, size)
    

    def set_size(self, size):

        status, n_signals = self._ocp_if.trace_get_number_signals(self._tr_id)
        if status < 0:
            return (-1, status)
        time.sleep(0.025)

        tr_size = size * n_signals

        return self._ocp_if.trace_set_size(self._tr_id, tr_size)
