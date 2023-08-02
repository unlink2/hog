#!/bin/bash

if [[ -z "${MEM}" ]]; then
  premake5 gmake && make && ./bin/Debug/testhog
else 
  premake5 gmake && make &&
    valgrind --leak-check=full --track-origins=yes ./bin/Debug/testhog
fi

