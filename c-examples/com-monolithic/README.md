# COM Monolithic Example

A TAB example for the COM board. The program is monolithic (no bootloader or
application, just a single program).

Usage:
* Ensure the dependencies outlined in the C examples [README](../README.md) are
  installed.
* Ensure the [setup_dependencies.sh](../scripts/setup_dependencies.sh) script
  has run successfully.
* Then execute the following `bash` commands:

```bash
cd ../scripts/
source sourcefile.txt
cd ../com-monolithic/
make
st-flash write com-monolithic.bin 0x????????
```

## License

Written by Bradley Denby  
Other contributors: None

See the top-level LICENSE file for the license.
