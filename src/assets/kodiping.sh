#!/bin/bash
# kodiping.sh <ip> <basic auth>(optional)
res=""
if [ -z "$2" ]
then
  res=$(curl --write-out "%{http_code}" -XHEAD --max-time 1 -s "$1/jsonrpc")
else
  res=$(curl --write-out "%{http_code}" -XHEAD -u "$2" --max-time 1 -s "$1/jsonrpc")
fi

if [ "$res" = "200" ]; then
  exit 0
fi

exit 1