#!/bin/zsh
format=eps
figs=(aaaa.fig merge.fig mississippi-0.fig mississippi-nodes mississippi-1)
transfig -L $format $figs
make

