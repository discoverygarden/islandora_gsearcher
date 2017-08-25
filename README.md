# Islandora GSearcher

## Introduction

Sends created and edited objects to be indexed via the Fedora Generic Search
Service on page exit.

## Requirements

This module requires the following modules/libraries:

* [Islandora](https://github.com/islandora/islandora)
* [Tuque](https://github.com/islandora/tuque)

## Installation

Install as usual, see
[this](https://drupal.org/documentation/install/modules-themes/modules-7) for
further information. Depending on your use case you may wish to deactivate the
use of ActiveMQ to greatly reduce server load during ingest.

To do so you would:
* remove the fedoragsearch.updaterNames property from fedoragsearch.properties.
  Some versions of GSearch (confirmed on 2.8) have issue with this change and
  start white screening.
* remove the ActiveMQ module section from fedora.fcfg

## Troubleshooting/Issues

Having problems or solved a problem? Contact 
[discoverygarden](http://support.discoverygarden.ca).

## Maintainers/Sponsors

Current maintainers:

* [discoverygarden](http://www.discoverygarden.ca)

Sponsors:

* [United States Department of Agriculture: National Agricultural Library](https://www.nal.usda.gov/)

## Development

If you would like to contribute to this module, please check out our helpful
[Documentation for Developers](https://github.com/Islandora/islandora/wiki#wiki-documentation-for-developers)
info, [Developers](http://islandora.ca/developers) section on Islandora.ca and
contact [discoverygarden](http://support.discoverygarden.ca).

## License

[GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)
