#!/bin/zsh

function plot() {
	output="$1.plot.png"
	input="$1.results"
	/usr/local/bin/gnuplot <<-END
		set terminal png color
		set title 'Number of nodes vs Text Size'
		set xlabel 'Number of Characters'
		set ylabel 'Number of nodes'


		set output "$output"
		plot '$input' using 1:2 with lines, '$input' using 1:3 with lines
	END
}

cd ../output || (echo "Error: no output directory" &1>2 ; exit 1)
plot dna
plot english
plot random
