#!/bin/bash

echo "Running: CMake"
cmake -DCMAKE_VERBOSE_MAKEFILE=1 -DOpenCL_INCLUDE_DIR=/usr/local/browndeer/include -DOpenCL_LIBRARY=/usr/local/browndeer/lib/libcoprthr.a .

echo "Do you wish to run make now?"
select yn in "Yes" "No"; do
    case $yn in
        Yes ) echo "Running: make"; make; break;;
        No ) exit;;
    esac
done

