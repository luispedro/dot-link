#!/bin/zsh

function do_test() {
	[[ -d output ]] || mkdir output
	input=textes/$1
	tinput=tmp/input
	toutput=tmp/output
	output=output/$1.results
	[[ -f $output ]] && rm $output
	for ((i=100;i<10001;i=i+100)); do
		echo -n .
		./textes/get-bytes $i < $input >$tinput
		./src/suffixtree f $tinput CACC >$toutput
		string_size=`grep 'String size:' $toutput | sed 's,[^0-9],,g'`
		nodes_without=`grep 'Nodes without' $toutput| sed 's,[^0-9],,g'`
		nodes_with=`grep 'Nodes with ' $toutput| sed 's,[^0-9],,g'`
		echo $string_size $nodes_without $nodes_with >>$output
	done
	echo
	rm $tinput
	rm $toutput
}

[[ -d tmp ]] || mkdir tmp
do_test dna
do_test random
do_test english
rmdir tmp

