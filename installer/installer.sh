#!/bin/bash

rm -f *.run
binarycreator --offline-only -c config/config.xml -p packages dtlslink-0.1-linux.run
