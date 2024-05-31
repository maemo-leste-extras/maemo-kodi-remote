#!/bin/bash

port=8080
hosts_up=$(nmap -oG - -n -sn -T5 --max-rtt-timeout 1s --min-parallelism 100 "$1" | grep Up | awk '{print $2}' | tr '\n' ' ')
hosts_port_up=$(nmap -oG - -sT -n -T5 --max-rtt-timeout 1s --min-parallelism 100 -p "$port" $hosts_up | grep '/open/' | awk '{print $2}')

for host in $hosts_port_up
do
  if ( curl --max-time 1 -svv "http://$host:$port" 2>&1 | grep 'realm="Kodi"' ) >/dev/null; then
    echo "$host:$port"
  fi
done
