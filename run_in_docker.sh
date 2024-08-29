#!/bin/bash
set -e

#!/bin/bash
usr="--user $(id -u $USER):$(id -g $USER)"

docker run -it $usr -e HOME -v $HOME:$HOME -w $PWD -e USER spoca "$@"
