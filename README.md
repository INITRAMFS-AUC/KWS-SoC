# Verilator Example
Minimal Example for a Verilated model and VPI setup

## Compilation
First Install Verilator

Ubuntu
```
sudo apt-get install verilator
```

Arch Linux Based Systems
```
sudo pacman -S verilator
```

Second, Compile with `--vpi` flag
```
verilator --cc --exe --vpi --build -j 0 -Wall sim_main.cpp our.v
```

## Running
Execute the compiled binary.
```
./obj_dir/Vour
```
