import os

import plecsutil as pu
import model


# --- Input ---
plecs_file = 'lrs_fs_buck_boost'
plecs_file_path = os.path.abspath(os.getcwd())

# --- Generates file ---
pm = pu.ui.PlecsModel(
    plecs_file, plecs_file_path,
    model.params(),
    )

pm.gen_m_file()

