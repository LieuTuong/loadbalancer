#!/bin/bash

scl enable devtoolset-7 bash
cd /loadbalancer/loadbalancer_src && make
exec /loadbalancer/loadbalancer_src/run 
