input=file('file')
suffixes=[]

string=input.readline().strip() + '$'
for i in xrange(len(string)):
    suffixes.append((string[i:],i))

suffixes.sort()
naive=[i for s,i in suffixes]
	
from os import popen
import re
input=popen('./suffixtree 1 file issi')
num = re.compile('^\d+$')
stree=[]
for line in input: 
	if num.match(line):
		stree.append(int(line))

if naive != stree:
	print "Error"
else:
	print "OK"
