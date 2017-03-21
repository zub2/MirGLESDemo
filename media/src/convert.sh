#!/bin/bash
set -e

# change to the directory where the script is stored
cd `dirname $0`

rsvg-convert -w 1000 -a die.svg -o ../die.png
