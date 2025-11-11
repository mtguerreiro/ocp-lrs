import time
import pyocp_lrs
import numpy as np
import matplotlib.pyplot as plt
plt.ion()

# --- Input ---
# Controller interface
port = 8080
host = "131.246.75.172"
settings = {'host':host, 'port':port}

buck = pyocp_lrs.buck.iface.Interface('ethernet', settings, cs_id=0, tr_id=0)

def run_exp():
    buck.set_ref(4); time.sleep(0.1)
    buck.idle.enable(); time.sleep(0.1)
    buck.enable(); time.sleep(0.1)
    
    buck.hw.set_input_relay(1); time.sleep(0.1)
    buck.hw.set_output_relay(1); time.sleep(0.1)
    buck.ramp.set_params({'u_ref': 0.4}); time.sleep(0.1)
    buck.ramp.enable(); time.sleep(0.1)

    mp = buck.sfb.get_model_params()
    mp.v_in = 16
    mp.L = 47e-6
    mp.Co = 560e-6
    mp.R = 10
    buck.sfb.set_model_params(mp)

    dt = 1 / 100e3
    buck.sfb.set_gains(ts=1e-3, os=5, dt=dt); time.sleep(0.1)

    buck.trace.set_mode(1); time.sleep(0.1)
    buck.trace.set_size(2000); time.sleep(0.1)
    buck.trace.set_n_pre_trig_samples(100); time.sleep(0.1)
    buck.trace.set_trig_signal(7); time.sleep(0.1)
    buck.trace.set_trig_level(5); time.sleep(0.1)
    buck.trace.reset(); time.sleep(0.1)

    buck.sfb.reset(); time.sleep(0.1)
    buck.sfb.enable(); time.sleep(0.1)
    time.sleep(0.25)

    buck.set_ref(6)
    time.sleep(0.25)

    buck.ramp.set_params({'u_ref': 0.0}); time.sleep(0.1)
    buck.ramp.enable(); time.sleep(0.1)

    status, data = buck.trace.read()
    t = dt * np.arange(data.shape[0]) / 1e-3

    plt.figure(); plt.plot(t, data[:, 4]); plt.plot(t, data[:, -1])

    return t, data

