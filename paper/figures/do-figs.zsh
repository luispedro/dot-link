#!/bin/zsh
format=eps
fig=(aaaa.fig merge.fig mississipi-0.fig)

transfig -L $format $figs
transfig -L $format -m .5 mississipi-nodes
transfig -L $format -m .25 mississipi-1


make

