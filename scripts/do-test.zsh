#!/bin/zsh

max=100000
step=100
k=1
function do_test() {
	[[ -d output ]] || mkdir output
	input=textes/$1
	tinput=tmp/input
	toutput=tmp/output
	output=output/"machine=`hostname`.in=$1.k=$k.results"
	[[ -f $output ]] && rm $output
	for (( i=$step; i<$max+1; i=i+$step )); do
		echo -n .
		./textes/get-bytes $i < $input >$tinput
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

[[ -d tmp ]] || mkdir tmp
do_test dna
do_test random
do_test english
rmdir tmp

