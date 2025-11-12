#!/bin/bash
# ===============================
# Flash CCS projects using Uniflash
# ===============================

# --- Default config variables ---
TI_ROOT="/home/marco/ti/ccs2031"
WORKSPACE="/home/marco/Desktop/ccs_ws"

# --- Parse command-line arguments ---
for arg in "$@"; do
  case $arg in
    --ti_root=*)
      TI_ROOT="${arg#*=}"
      shift
      ;;
    --workspace=*)
      WORKSPACE="${arg#*=}"
      shift
      ;;
    --help)
      echo "Usage: $0 [--ti_root=PATH] [--workspace=PATH]"
      exit 0
      ;;
    *)
      echo "Unknown option: $arg"
      echo "Usage: $0 [--ti_root=PATH] [--workspace=PATH]"
      exit 1
      ;;
  esac
done

# --- Paths ---
UNIFLASH_CLI="$TI_ROOT/ccs/ccs_base/scripting/examples/uniflash/cmdLine/uniflash.sh"

# --- Project paths ---
CPU1_CCXML="$WORKSPACE/empty_driverlib_project_cpu1/targetConfigs/TMS320F28377D.ccxml"
CPU2_CCXML="$WORKSPACE/empty_driverlib_project_cpu2/targetConfigs/TMS320F28377D.ccxml"

CPU1_OUT="$WORKSPACE/empty_driverlib_project_cpu1/CPU1_FLASH/empty_driverlib_project_cpu1.out"
CPU2_OUT="$WORKSPACE/empty_driverlib_project_cpu2/CPU2_FLASH/empty_driverlib_project_cpu2.out"

# --- Flash CPU1 ---
echo ">>> Flashing CPU1..."
"$UNIFLASH_CLI" \
    -ccxml "$CPU1_CCXML" \
    -core C28xx_CPU1 \
    -program "$CPU1_OUT"

# --- Flash CPU2 ---
echo ">>> Flashing CPU2..."
"$UNIFLASH_CLI" \
    -ccxml "$CPU2_CCXML" \
    -core C28xx_CPU2 \
    -program "$CPU2_OUT"
