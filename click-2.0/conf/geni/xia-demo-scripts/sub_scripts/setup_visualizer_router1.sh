#!/bin/bash

sudo killall -9 updateForwardingRate.py
sudo killall -9 updateState.py

sleep 1
bash ~/xia-core/click-2.0/conf/geni/stats/xia-link-state.sh | ~/xia-core/click-2.0/conf/geni/visualizer-scripts/updateForwardingRate.py R0-R1 ganel.gpolab.bbn.com &
echo Forwarding | ~/xia-core/click-2.0/conf/geni/visualizer-scripts/updateState.py R1 ganel.gpolab.bbn.com &

