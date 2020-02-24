shell echo gdb-multiarch -x .dbgpkg/.gdbscript
shell export PYTHONPATH=~/src/FreeRTOS-GDB/src/
source .dbgpkg/.gdbscript 
source ~/src/FreeRTOS-GDB/src/FreeRTOS.py

dash -layout source breakpoints expressions registers stack threads variables