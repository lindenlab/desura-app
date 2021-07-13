#!/bin/bash

# Stolen from http://stackoverflow.com/a/246128/292831
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ] ; do SOURCE="$(readlink "$SOURCE")"; done
DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

# export PATH with DIR so we add the binary folder to PATH (for desura_bittest calls)
export PATH="$DIR:$PATH"

export XDG_CONFIG_HOME="$HOME/.config"
export XDG_CACHE_HOME="$HOME/.cache"

if [[ "@RUNTIME_LIB_INSOURCE_DIR@" = /* ]]; then
	export LD_LIBRARY_PATH="@RUNTIME_LIB_INSOURCE_DIR@/"
	# also add our runtime lib directory to path
	export PATH="$LD_LIBRARY_PATH:$PATH"
	$DESURA_DEBUGGERS @RUNTIME_LIB_INSOURCE_DIR@/@CURRENT_TARGET@ $@
else
	export LD_LIBRARY_PATH="$DIR/@RUNTIME_LIB_INSOURCE_DIR@/"
	# also add our runtime lib directory to path
	export PATH="$LD_LIBRARY_PATH:$PATH"
	$DESURA_DEBUGGERS "$DIR/@RUNTIME_LIB_INSOURCE_DIR@/@CURRENT_TARGET@" $@
fi

