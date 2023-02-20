# TAB

This repository includes documentation and reference implementations for TAB,
which is a serial communication protocol for satellite commands and data. TAB is
inspired by, and largely compatible with, the
[OpenLST](https://github.com/OpenLST/openlst) serial communication protocol for
satellites. The
[Tartan Artibeus Smallsat'22 paper](https://digitalcommons.usu.edu/smallsat/2022/all2022/54/)
describes the origins and goals of TAB, which stands for "Tartan Artibeus Bus."
TAB allows research satellites to accomplish three goals:
* TAB supports deployment of research concepts to satellites in orbit. Because
  TAB provides a means to remotely command and control satellites, an operator
  can use TAB to instruct satellites in space to act out research concepts and
  report the results for evaluation. Thus, TAB supports *accessibility* via
  ground control of deployed satellites.
* TAB supports hardware-in-the-loop mission simulation with flight hardware.
  Using TAB, mission simulation software sends commands and data to a test
  satellite (hardware-in-the-loop). Also using TAB, the test satellite generates
  replies and sends these replies to the mission simulation software in real
  time. Thus, TAB supports *compatibility* with hardware-in-the-loop simulation.
* TAB supports intermodule communication and easy integration of third-party
  modules. By including the TAB reference implementation, a third-party module
  or custom payload immediately gains the ability to interact with the rest of
  the satellite. Thus, TAB supports *extensibility* via straightforward
  communication among independently-designed modules.

**Current version**: 0.0.0

* This software uses [semantic versioning](http://semver.org).

**Dependencies**

* C examples:
```bash
sudo apt install build-essential cmake gcc git libusb-1.0-0 libusb-1.0-0-dev
cd /path/to/tab/c-examples/scripts/
./setup_dependencies.sh
sudo cp ../utilities/stlink/config/udev/rules.d/*.rules /etc/udev/rules.d/
sudo shutdown --reboot now
```

* Python examples:
```bash
sudo apt install python3-venv
sudo usermod -aG dialout $USER
sudo shutdown --reboot now
cd /path/to/tab/python-examples/scripts/
./setup_dependencies.sh
```

## Directory Contents

* [c-examples](c-examples/README.md): TAB examples using the C implementation
* [c-implementation](c-implementation/README.md): C implementation of TAB
* [images](images/README.md): Contains image files used for documentation or
  other purposes
* [python-examples](python-examples/README.md): TAB examples using the Python
  implementation
* [python-implementation](python-implementation/README.md): Python
  implementation of TAB
* [CHANGELOG.md](CHANGELOG.md): A log of changes made to the repository
* [CONTRIBUTING.md](CONTRIBUTING.md): Rules for contributing to the repository
* [DOCUMENTATION.md](DOCUMENTATION.md): TAB documentation
* [LICENSE](LICENSE): License
* [README.md](README.md): This document

## License

Written by Bradley Denby  
Other contributors: None

See the top-level LICENSE file for the license.
