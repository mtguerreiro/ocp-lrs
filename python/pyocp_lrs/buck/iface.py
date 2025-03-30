import struct

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

