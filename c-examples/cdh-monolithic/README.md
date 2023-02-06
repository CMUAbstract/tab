# CDH Monolithic Example

A TAB example for the CDH board. The program is monolithic (no bootloader or
application, just a single program).

Usage:
* Ensure the dependencies outlined in the C examples [README](../README.md) are
  installed.
* Ensure the [setup_dependencies.sh](../scripts/setup_dependencies.sh) has run
  successfully.
* Then execute the following `bash` commands:

```bash
cd ../scripts/
source sourcefile.txt
cd ../cdh-monolithic/
make
st-flash write cdh-monolithic.bin 0x08000000
```

## License

Written by Bradley Denby  
Other contributors: None

See the top-level LICENSE file for the license.
