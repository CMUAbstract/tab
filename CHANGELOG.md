# Change log

All notable changes to the TAB repository are documented in this file. See the
bottom of the file for a change log entry template.

## Overview

* [Unreleased](#unreleased)
* [0.0.0](#0.0.0)
* [Template](#template)
* [License](#license)

## <a name="unreleased"></a> Unreleased

### Added
* Descriptions of the Common Ack, Common Nack, Common Debug, and Common Data
  commands to the documentation file
* C implementation files
* A directory for a CDH board "monolithic" TAB example
* Utilities and scripts supporting C examples
* A Python implementation file
* Directories for Python RX and TX examples
* A directory for a COM board "monolithic" TAB example
* Clock and LED initialization functions for the monolithic COM board example
* Descriptions of the Bootloader Ack, Bootloader Nack, Bootloader Ping, and
  Bootloader Erase commands to the documentation file
* Support for COM board UART using libopencm3 library
* Bootloader Write Page, Bootloader Write Page Addr32, and Bootloader Jump
  support in the CDH example and skeleton functions in the COM example
* Adds LED initialization to CDH example
* Toggles CDH LEDs after tranmission of each complete TAB reply
* Toggles COM LEDs after tranmission of each complete TAB reply
* Bootloader Erase support in the COM example
* Bootloader Write Page support in the COM example

### Fixed
* Fixes Markdown table styling in the documentation file
* Fixes initial commits of monolithic CDH C example and Python TX example
* Fixes CDH Common Data handler implementation
* Adds additional details to documented C example dependencies
* Adds additional details to documented Python example dependencies
* LED initialization in the COM example

## <a name="0.0.0"></a> 0.0.0

### Added
* Documentation, change log, contribution guide, license, README, and .gitignore
* Directories and READMEs for C and Python examples and implementations, images

## <a name="template"></a> Template

### Added
* New features

### Changed
* Changes to existing functionality

### Deprecated
* Stable features being removed in an upcoming release

### Removed
* Depreated features removed in this release

### Fixed
* Fixes

### Security
* Upgrade recommendations as a result of removed vulnerabilities

## <a name="license"></a> License

Written by Bradley Denby  
Other contributors: None

See the top-level LICENSE file for the license.
