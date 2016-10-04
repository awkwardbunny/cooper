#!/bin/bash
i="1"
while [ $i -ne 524188 ]
do
    echo Timing buffer size of $i bytes... >> $2
    { time ./copycat -b $i $1 -o $i.output; } 2>> $2
    i=$[$i*2]
done
