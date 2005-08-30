#!/bin/zsh

function plot() {
	output="$1.plot.png"
	input="$1"
	gnuplot <<-END
		set terminal png color
		set title 'Number of nodes vs Text Size'
		set xlabel 'Number of Characters'
		set ylabel 'Number of nodes'


		set output "$output"
		plot '$input' using 1:2 with lines, '$input' using 1:3 with lines
		set output "ratio-$output"
		plot  '$input' using 1:(\$3/\$2) with lines
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
	plot $f
done
