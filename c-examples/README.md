# C Examples

TAB examples using the C implementation

**Dependencies**

```bash
sudo apt install build-essential cmake gcc git libusb-1.0-0 libusb-1.0-0-dev
cd /path/to/tab/c-examples/scripts/
./setup_dependencies.sh
sudo cp ../utilities/stlink/config/udev/rules.d/*.rules /etc/udev/rules.d/
sudo shutdown --reboot now
```

## Directory Contents

* [cdh-monolithic](cdh-monolithic/README.md): A simple example program for the
  CDH board
* [com-monolithic](com-monolithic/README.md): A simple example program for the
  COM board
* [scripts](scripts/README.md): Scripts supporting TAB C examples
* [utilities](utilities/README.md): Utilities for TAB C examples
* [README.md](README.md): This document

## License

Written by Bradley Denby  
Other contributors: None

See the top-level LICENSE file for the license.
