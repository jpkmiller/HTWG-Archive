#!/bin/bash

echo "18" >"/sys/class/gpio/export"
echo "out" >"/sys/class/gpio/gpio18/direction"

while true; do
  echo "LED: on"
  echo "1" >"/sys/class/gpio/gpio18/value"
  sleep 1
  echo "LED: off"
  echo "0" >"/sys/class/gpio/gpio18/value"
  sleep 1
done

trap "echo 18 > /sys/class/gpio/unexport" EXIT
