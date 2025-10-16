import pyocp_lrs

#import pyocp_lrs.fsbuckboost.boost_tb as btb
#import pyocp_lrs.fsbuckboost.buck_tb as btb

import pyocp

import numpy as np
import matplotlib.pyplot as plt
plt.ion()

import time
import os, subprocess, platform


# --- Input ---



# Controller interface
port = 8080
host = '131.246.75.214'
settings = {'host':host, 'port':port}

fsbb = pyocp_lrs.fsbuckboost.iface.Interface('ethernet', settings, cs_id=0, tr_id=0)
globals()['fsbb'] = fsbb