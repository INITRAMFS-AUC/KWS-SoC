# `altera-compat` PR

- added a new `--auto-vh` flag in `Hazard3/scripts/listfiles` to automatically include `.vh` files within flat file lists
- Changed Makefile to accomodate changes in `listfiles` script, this added the new `quartus_prep` target for `make`. 

>[!IMPORTANT]
> `quartus_prep` target makes symbolic links of the required dependencies of the specified `.f` file in the `Makefile`, and puts it in an ignored directory `quartus_work_dir`.
> This ensures any changes are done to the original files, superior to copying files raw.

## How to prep the repo

1. Run the new target: 

```
make quartus_prep
```

2. Open a new Quartus Project and select the working directory to be `quartus_work_dir`

3. Add all the files in that directory (except the ones Quartus puts of course)


