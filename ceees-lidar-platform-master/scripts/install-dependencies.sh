#!/bin/bash

tar xf ../misc/gcc-8.1.0.tar.bz2
sudo mv ./gcc-8.1.0 /usr/local
echo 'export PATH=/usr/local/gcc-8.1.0/bin:$PATH' >> .bashrc

unzip ../misc/CML.zip
make -C ./CML/

unzip ../misc/copleycan-1.04.0.zip

unzip ../misc/vnproglib-1.1-cpp.zip
