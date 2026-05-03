 
## Useful script workflows

### FPGA Validation



### ASIC

Linking your source file elsewhere (OpenLane for example):

 ```bash
 python ./scripts/flist_link.py --l <(python3 ./scripts/listfiles -f flat --auto-vh soc.f) -d <OpenLane design path>
 ```
