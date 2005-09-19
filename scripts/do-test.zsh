#!/bin/zsh

step=100
function do_test() {
	[[ -d output ]] || mkdir output
	echo do_test $@
	local input=textes/$1
	local tinput=tmp/input
	local toutput=tmp/output
	local k=$2
	local max=$3
	local i
	local output=output/"machine=`hostname`.in=$1.k=$k.max=$max.results"
	[[ -f $output ]] && rm $output
	for (( i=$step; i<$max+1; i=i+$step )); do
		echo -n .
		./textes/get-bytes.py $i < $input >$tinput
		./src/suffixtree $k $tinput CACC >$toutput
		string_size=`grep 'String size:' $toutput | sed 's,[^0-9],,g'`
		nodes_without=`grep 'Nodes without' $toutput| sed 's,[^0-9],,g'`
		nodes_with=`grep 'Nodes with ' $toutput| sed 's,[^0-9],,g'`
		time_full=`grep 'timer\[full' $toutput | sed 's,[^0-9],,g'`
		time_add=`grep 'timer\[add' $toutput | sed 's,[^0-9],,g'`
		echo $string_size $nodes_without $nodes_with $time_full $time_add >>$output
	done
	echo
	echo
	rm $tinput
	rm $toutput
}

files=(english)
ks=(1 2 3)
maxs=(100000 100000 50000)

[[ -d tmp ]] || mkdir tmp
for f in $files; do
	for (( i=1; i <= $#ks; i=i+1 )); do
		do_test $f $ks[$i] $maxs[$i]
	done
done
rmdir tmp

