#!/bin/zsh

files=(dna english random)
ks=(2)
step=2
mmax=100
minit=4
searchrepeat=1000
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
	local k=$2
	local tpat=tmp/patterns
	local toutput=tmp/output
	local output=output/"search-vary-M-machine=`hostname`.in=$1.k=$k.max=full.results"
	local ttoutput="$output.tmp"
	local m

	[[ -f $ttoutput ]] && rm $ttoutput

	for (( m=$minit; m<$mmax+1; m=m+$step )); do
		echo -n .
		echo "timer ok m=$m" >>$tpat
		$fileportion $input . $m $k $searchrepeat >>$tpat
		echo "timer not m=$m" >>$tpat
		$fileportion $input . $m $((k+1)) $searchrepeat >>$tpat
	done
	echo
	echo -n "Running suffixtree...  "
	./src/suffixtree $k $input - <$tpat | grep 'Accumulator' >$toutput
	echo "done"
	for (( m=$minit; m < $mmax + 1; m=m+$step )); do
		local ok_steps=`get_number 'Accumulator .ok m='$m'\]' $toutput`
		local ok_time=`get_number 'Accumulator .timer.ok m='$m'\]' $toutput`
		local not_steps=`get_number 'Accumulator .not m='$m'\]' $toutput`
		local not_time=`get_number 'Accumulator .timer.not m='$m'\]' $toutput`
		echo $m $ok_steps $ok_time $not_steps $not_time >>$ttoutput
	done
	echo
	rm $tpat
	rm $toutput
	mv $ttoutput $output
}


[[ -d tmp ]] || mkdir tmp
for f in $files; do
	for (( i=1; i <= $#ks; i=i+1 )); do
		do_test $f $ks[$i]
	done
done
rmdir tmp

