# Verilator Example
Minimal Example for a Verilated model and VPI setup

## Building

### Prerequisites
First Install Verilator

Ubuntu
```
sudo apt-get install verilator
```

Arch Linux Based Systems
```
sudo pacman -S verilator
```

### Building

>[!Important]
> Currently the build produces a lot of warnings, but example soc is functional.

```
make verilate
```

```
make verilate_no_warnings
```

## Running
Execute the compiled binary at ``obj/dir/sim_main``

or to build and run immediately:

```
make run
```

```
make run_no_warnings
```
