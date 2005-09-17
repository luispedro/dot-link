#!/bin/zsh


# For both
repeats=10000

# For vary_k
m=50
kmax=4

#For vary_m
mstep=2
mmin=6
mmax=100
kfixed=2

inputs=(english dna random)
fileportion=./scripts/get-fileportion.py


function vary_k() {
	input=textes/$1
	output=tmp/patterns-vary-k.in=$1
	rm -f $output
	for (( i=0; i < $kmax+2; i=i+1 )); do
		echo "timer k=$i" >>$output
		$fileportion $input . $m $i $repeats >>$output
	done
}

function vary_m() {
	input=textes/$1
	output=tmp/patterns-vary-m.in=$1.k=$kfixed
	rm -f $output
	for (( i=$mmin; i < $mmax+1; i=i+mstep )); do 
		echo "timer m=$i.ok" >> $output
		$fileportion $input . $i $kfixed $repeats >> $output
		echo "timer m=$i.not-ok" >> $output
		$fileportion $input . $i $((kfixed+1)) $repeats >> $output
	done
}

for t in $inputs; do 
	vary_m $t-100k
	#vary_k $t-100k
done
