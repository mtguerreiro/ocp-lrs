#!/bin/bash
# ===============================
# Build CCS projects
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
CCS_CLI="$TI_ROOT/ccs/eclipse/ccs-server-cli.sh"

# --- Build Projects ---
echo ">>> Cleaning workspace: $WORKSPACE"
"$CCS_CLI" -noSplash \
    -workspace "$WORKSPACE" \
    -application com.ti.ccs.apps.buildProject \
    -ccs.workspace \
    -ccs.clean

echo ">>> Building workspace: $WORKSPACE"
"$CCS_CLI" -noSplash \
    -workspace "$WORKSPACE" \
    -application com.ti.ccs.apps.buildProject \
    -ccs.workspace
