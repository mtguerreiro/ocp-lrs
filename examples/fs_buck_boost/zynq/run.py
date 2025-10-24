import pyocp_lrs
import pyocp

import numpy as np
import matplotlib.pyplot as plt
plt.ion()

import time

# --- Input ---
port = 8080
host = '131.246.75.171'
settings = {'host':host, 'port':port}

fsbb = pyocp_lrs.fsbuckboost.iface.Interface('ethernet', settings, cs_id=0, tr_id=0)
globals()['fsbb'] = fsbb
