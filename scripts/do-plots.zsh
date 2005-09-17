#!/bin/zsh

ext=eps
gnuplotterminal='postscript eps enhanced color'

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
	input="pasted-in=$dataset.tmp"
	paste *.in=$dataset*max=100000*.results >$input
	gnuplot <<-END
		set terminal $gnuplotterminal
		set title 'No errors to 1 error'
		set xlabel 'Number of Characters'
		set ylabel 'Ratio'
		set yrange[0:]

		set output "ratio-in=$dataset-0-1.$ext"
		plot '$input' using 1:(\$3/\$2) with lines title 'Nodes'

		set title '1 error to 2 errors'
		set output "ratio-in=$dataset-1-2.$ext"
		plot '$input' using 1:(\$8/\$3) with lines title 'Nodes'

		set title '2 error to 3 errors'
		set output "ratio-in=$dataset-2-3.$ext"
		plot '$input' using 1:(\$13/\$8) with lines title '2 Errors to 3 Errors'
	END
	rm $input
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


