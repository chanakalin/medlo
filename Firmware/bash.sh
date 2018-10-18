#!/bin/bash

while true; do
	date +%F_%H%M%S
	mosquitto_pub -h iot.eclipse.org -p 1883 -t carbon.super/medlonode/0/configranges -m "$(date +%F_%H%M%S)"
	sleep 10
done
