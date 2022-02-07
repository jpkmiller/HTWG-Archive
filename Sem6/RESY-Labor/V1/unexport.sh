#!/bin/sh
for Port in 17 18
  do
  echo "$Port" > /sys/class/gpio/unexport
done