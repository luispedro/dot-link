#!/bin/zsh

files=(dna english random)
ks=(2)
maxs=(100000)
step=1000
searchrepeat=10000
m=15
fileportion=scripts/get-fileportion.py

get_number() {
	local pat=$1
	local file=$2
	echo `egrep $pat $file | sed 's,[^0-9],,g'`
}

function do_test() {
	[[ -d output ]] || mkdir output
	echo do_test $@
	local input=textes/$1
	local tinput=tmp/input
	local tpat=tmp/patterns
	local toutput=tmp/output
	local k=$2
	local max=$3
	local n
	local output=output/"search-vary-N-machine=`hostname`.in=$1.k=$k.max=$max.results"
	[[ -f $output ]] && rm $output

	for (( n=$step; n<$max+1; n=n+$step )); do
		echo -n .
		./textes/get-bytes.py $n < $input >$tinput

		[[ -f $tpat ]] && rm $tpat
		#./textes/get-bytes.py $step $input >$tinput
		echo 'timer ok' >>$tpat
		$fileportion $tinput . $m $k $searchrepeat >>$tpat
		echo 'timer not' >>$tpat
		$fileportion $tinput . $m $((k+1)) $searchrepeat >>$tpat


		./src/suffixtree $k $tinput - <$tpat | tail -n20 >$toutput
		local string_size=`get_number 'String size:' $toutput`
		local ok_steps=`get_number 'Accumulator .ok' $toutput`
		local ok_time=`get_number 'Accumulator .timer.ok' $toutput`
		local not_steps=`get_number 'Accumulator .not' $toutput`
		local not_time=`get_number 'Accumulator .timer.not' $toutput`
		echo $string_size $ok_steps $ok_time $not_steps $not_time >>$output
	done
	echo
	echo
	rm $tinput
	rm $tpat
	rm $toutput
}


[[ -d tmp ]] || mkdir tmp
for f in $files; do
	for (( i=1; i <= $#ks; i=i+1 )); do
		do_test $f $ks[$i] $maxs[$i]
	done
done
rmdir tmp

