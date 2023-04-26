#!/bin/bash

# Loop through the command-line arguments
for arg in "$@"
do
    case $arg in
        driver)
            make -C driver/simx/ clean && make -C driver/simx/
            ;;
        simx)
            make -C sim/simx clean && make -C sim/simx
            ;;
        kernel)
            make -C tests/regression/brainfloat/ clean-all && make -C tests/regression/brainfloat/
            ;;
        run)
            ./ci/blackbox.sh --driver=simx --cores=4 --app=brainfloat
            ;;
        out)
            ./ci/blackbox.sh --driver=simx --cores=4 --app=brainfloat > dog2.log
            ;;
        *)
            echo "Invalid argument: $arg"
            ;;
    esac
done