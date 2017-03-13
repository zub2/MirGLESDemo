#!/bin/bash
set -e

# change to the directory where the script is stored
cd `dirname $0`

if [ -e glm ]; then
	echo "Skipping glm because it's been downloaded already."
else
	echo "Downloading glm..."

	# shallow clone tag 0.9.8.4
	git clone --depth 1 --branch 0.9.8.4 https://github.com/g-truc/glm.git glm
fi
