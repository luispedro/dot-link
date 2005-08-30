#!/usr/bin/env python
from sys import argv
import random
import os

inputfile=argv[1]
filelen=os.stat(inputfile)[6]

lenght=int(argv[3])
k=int(argv[4])

if argv[2] == '.':
	begin = random.randrange(filelen-lenght)
else:
	begin=int(argv[2])
	if begin + lenght > filelen:
		raise Exception('File not large enough')

f=open(inputfile,'r')
f.seek(begin)
string = f.read(lenght)

changed = []
for point in random.sample(range(0,len(string)),k):
	# '#' is a character which does not show up in any of our data files
	if point > 0:
		string = string[0:point-1] + '#' + string[point:]
	else:
		string = '#' + string[1:]

print string
