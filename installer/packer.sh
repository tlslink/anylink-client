#!/bin/bash

rm -f *.run
rm -f *.gz

binarycreator --offline-only -c config/config.xml -p packages anylink-linux-amd64.run
