#!/usr/bin/env python
from sys import argv
import sys
import random
import os

inputfile=argv[1]
filelen=os.stat(inputfile)[6]

lenght=int(argv[3])
k=int(argv[4])

f=open(inputfile,'r')
repeats=1
if len(argv) > 5:
	repeats=int(argv[5])

def get_string(f,begin,lenght,k):
	f.seek(begin)
	string = f.read(lenght)
	for point in random.sample(range(0,len(string)),k):
		# '#' is a character which does not show up in any of our data files
		if point > 0:
			string = string[0:point-1] + '#' + string[point:]
		else:
			string = '#' + string[1:]

	print string


for i in range(repeats):
	if argv[2] == '.':
		begin = random.randrange(filelen-lenght)
	else:
		begin=int(argv[2])
		if begin + lenght > filelen:
			raise Exception('File not large enough')
	get_string(f,begin,lenght,k)

