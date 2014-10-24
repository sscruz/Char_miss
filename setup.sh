#!/bin/bash

ker=$(uname -r) 
kersc5=2.6.18-371.8.1.el5


if [ "$ker" == "$kersc5" ]; then
    echo Setting up variables for SC5
    source /nfs/fanae/root_releases/root.5.34.13.slc5/bin/thisroot.sh
    source /nfs/fanae/PAF_releases/PAF_devel/PAF_setup.sh 
    source /opt/PoD/PoD_env.sh 
else
    echo Setting up variables for SC6
    source /nfs/fanae/root_releases/root.5.34.13.slc6/bin/thisroot.sh
    source /nfs/fanae/PAF_releases/PAF_devel/PAF_setup.sh 
    source /opt/PoD/PoD_env.sh 
fi