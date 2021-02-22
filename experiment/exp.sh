#!/bin/bash
for batch_size in {2..60}
do LD_PRELOAD=$PWD/../user/preload.so ./write 60 | tail -n 1 >> db3.out
#do ./write 60 | tail -n 1 >> n3.out
done
