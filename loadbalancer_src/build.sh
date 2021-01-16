#!/bin/bash
rm -f /loadbalancer/loadbalancer_logs/*
rm -f run netdcli netdsrv
make > /dev/null
g++ netdclient.cpp -o netdcli
g++ netdsrv.cpp -o netdsrv
systemctl restart lb netdsend netdrecv
