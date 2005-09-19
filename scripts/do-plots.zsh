#!/bin/zsh

ext=eps
gnuplotterminal='postscript eps enhanced'

function plot() {
	output="$1.plot.$ext"
	input="$1"
	gnuplot <<-END
		set terminal $gnuplotterminal
		set title 'Number of nodes vs Text Size'
		set xlabel 'Number of Characters'
		set ylabel 'Number of Nodes'


		set output "$output"
		plot '$input' using 1:2 with lines title 'Suffix Tree', '$input' using 1:3 with lines title 'Dotted Tree'
	END
}

function ratios() {
	dataset=$1
	local tinput="pasted-in=$dataset.tmp"
	paste machine=gauss.in=$dataset.k=*.max=100000.results >$tinput
	gnuplot <<-END
		set terminal $gnuplotterminal
		unset title
		set xlabel 'Number of Characters'
		set ylabel 'Ratio'
		set key bottom right reverse Left
		set yrange[0:]
		set size 1,.75
		set format x "%3gk"

		set output "ratio-in=$dataset-01-12.$ext"

		plot '$tinput' using (\$1/1000):(\$3/\$2) with lines title 'No errors to one error', \
			'$tinput' using (\$1/1000):(\$8/\$3) with lines title '1 error to 2 errors'
	END
	rm $tinput
}

function search() { 
	local input1="search-vary-N-machine=boole.in=dna.k=2.max=100000.results"
	local input2="search-vary-N-machine=boole.in=english.k=2.max=100000.results"
	local input3="search-vary-N-machine=boole.in=random.k=2.max=100000.results"
	gnuplot <<-END
		set terminal $gnuplotterminal
		set output 'search-vary-N.in=all.$ext'
		unset title
		set yrange[0:]
		set xlabel 'Text Size'
		set ylabel 'Character Comparisons (average)'
		set format x "%3gk"
		set ytics
		set key bottom right reverse Left
		set size 1,.75

		plot 	'$input1' using (\$1/1000):(\$2/10000) with lp title 'Existing strings, {/Italic dna}', \
			'$input2' using (\$1/1000):(\$2/10000) with lp title 'Existing strings, {/Italic english}', \
			'$input3' using (\$1/1000):(\$2/10000) with lp title 'Existing strings, {/Italic random text}', \
			'$input1' using (\$1/1000):(\$4/10000) with lp title 'Non-existing strings, {/Italic dna}', \
			'$input2' using (\$1/1000):(\$4/10000) with lp title 'Non-existing strings, {/Italic english}', \
			'$input3' using (\$1/1000):(\$4/10000) with lp title 'Non-existing strings, {/Italic random text}'
	END
}

if test -d ../output; then
	d=../output
elif test -d ./output; then
	d=./output
else
	echo "Error: no output directory" &1>2
	exit 1
fi

cd $d

for f in *.results; do
	echo plot $f
done

for f in english dna random; do
	ratios $f
done


search

