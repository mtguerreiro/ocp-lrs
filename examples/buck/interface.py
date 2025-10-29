import pyocp_lrs


# --- Input ---
# Controller interface
port = 8080
host = "169.254.177.200"
settings = {'host':host, 'port':port}

buck = pyocp_lrs.buck.iface.Interface('ethernet', settings, cs_id=0, tr_id=0)
