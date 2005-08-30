#!/bin/zsh

m=50
kmax=4
patterns=tmp/patterns
input=tmp/dna-small
fileportion=./scripts/get-fileportion.py

[[ -f $patterns ]] && rm $patterns 

for (( i=0; i < $kmax+2; i=i+1 )); do
	echo "timer k=$i" >>$patterns
	repeat 500 { $fileportion $input . $m $i } >>$patterns
done

