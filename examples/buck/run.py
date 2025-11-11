import time
import pyocp_lrs
import numpy as np
import matplotlib.pyplot as plt
plt.ion()

# --- Input ---
# Controller interface
port = 8080
host = "131.246.75.175"
settings = {'host':host, 'port':port}

buck = pyocp_lrs.buck.iface.Interface('ethernet', settings, cs_id=0, tr_id=0)
