#!/bin/zsh

files=(dna english random)
ks=(1 2)
N=100000
step=2
start=2
max=100
searchrepeat=10000
fileportion=scripts/get-fileportion.py
gen_random=textes/gen-random

get_number() {
	local pat=$1
	local file=$2
	echo `egrep $pat $file | sed 's,[^0-9],,g'`
}
get_nodes() {
	local sigma=$1
	local toutput=$2
	string_size=`grep 'String size:' $toutput | sed 's,[^0-9],,g'`
	nodes_without=`grep 'Nodes 0-order' $toutput| sed 's,[^0-9],,g'`
	nodes_with=`grep 'Nodes Allocated' $toutput| sed 's,[^0-9],,g'`
	time_full=`grep 'timer\[full' $toutput | sed 's,[^0-9],,g'`
	time_add=`grep 'timer\[add' $toutput | sed 's,[^0-9],,g'`
	echo $string_size $sigma $nodes_without $nodes_with $time_full $time_add
}

get_searches() {
	local sigma=$1
	local toutput=$2
	local string_size=`get_number 'String size' $toutput`
	local ok_steps_subst=`get_number 'Accumulator .ok.subst' $toutput`
	local ok_time_subst=`get_number 'Accumulator .timer.ok.subst\]' $toutput`
	local not_steps_subst=`get_number 'Accumulator .not.subst' $toutput`
	local not_time_subst=`get_number 'Accumulator .timer.not.subst\]' $toutput`
	local ok_steps_edit=`get_number 'Accumulator .ok.edit' $toutput`
	local ok_time_edit=`get_number 'Accumulator .timer.ok.edit\]' $toutput`
	local not_steps_edit=`get_number 'Accumulator .not.edit' $toutput`
	local not_time_edit=`get_number 'Accumulator .timer.not.edit\]' $toutput`
	echo $string_size $sigma $ok_steps_subst $ok_time_subst $not_steps_subst $not_time_subst $ok_steps_edit $ok_time_edit $not_steps_edit $not_time_edit
}

function do_test() {
	[[ -d output ]] || mkdir output
	local tinput=tmp/input
	local toutput=tmp/output
	local tpat=tmp/patterns
	local sigma
	local ttoutput_nodes="tmp/output.nodes.tmp"
	local ttoutput_searches="tmp/output.searches.tmp"
	local output_nodes=output/"vary-sigma-nodes-machine=`hostname`.k=$k.max=$max.results"
	local output_searches=output/"sigma-searches-machine=`hostname`.k=$k.max=$max.results"
	local k=$1
	local m=15
	[[ -f $ttoutput ]] && rm $ttoutput

	echo '#' string_size sigma nodes_without nodes_with time_full time_add >>$ttoutput_nodes
	echo '#' m ok_steps_subst ok_time_subst not_steps_subst not_time_subst \
			ok_steps_edit ok_time_subst not_steps_subst not_time_subst  >>$ttoutput_searches
	for (( sigma=$start; sigma<$max+1; sigma=sigma+$step )); do
		echo "$sigma ($k)...."
		$gen_random $N $sigma $sigma >$tinput

		[[ -f $tpat ]] && rm $tpat
		printf 'timer ok\0' >>$tpat
		$fileportion $tinput . $m $k $searchrepeat >>$tpat
		printf 'timer not\0' >>$tpat
		$fileportion $tinput . $m $((k+1)) $searchrepeat >>$tpat


		./src/suffixtree $k $tinput - <$tpat | tail -n30 >$toutput
		get_nodes $sigma $toutput >>$ttoutput_nodes
		get_searches $sigma $toutput >>$ttoutput_searches
	done

	echo
	echo done
	rm $tinput
	rm $toutput
	mv $ttoutput_nodes $output_nodes
	mv $ttoutput_searches $output_searches	
}


[[ -d tmp ]] || mkdir tmp
for k in $ks; do
	do_test $k
done
rmdir tmp

