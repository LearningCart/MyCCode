#!/bin/bash

name="base"
max=100
counter=0
inputfile="test.bin"

for i in `seq 1 $max`
do
	echo -n "Copying File test.bin to : $name$counter.bin"
	cp $inputfile "$name$counter.bin"
	echo "      DONE"
	counter=`expr $counter + 1`
done
