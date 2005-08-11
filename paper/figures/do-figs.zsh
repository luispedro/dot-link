#!/bin/zsh
format=eps
figs=(aaaa.fig merge.fig mississipi-0.fig mississipi-nodes mississipi-1)
transfig -L $format $figs
make

