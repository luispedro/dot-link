import sys

filename='file'
if len(sys.argv) >= 1:
    filename=sys.argv[1]

input=file(filename)

string=input.readline().strip()

naive=range(len(string))
def sufcmp(a,b):
	if a == b:
		return 0
	while a < len(string) and b < len(string) and string[a] == string[b]:
		a += 1
		b += 1
	if a == len(string):
		return -1
	if b == len(string):
		return +1
	if string[a] < string[b]:
		return -1
	return 1
naive.sort(sufcmp)
naive.insert(0,len(string)) # For the '\0' !
	
from os import popen
import re
input=popen('./suffixtree %s' % filename)
num = re.compile('^\d+$')
stree=[]
for line in input: 
	if num.match(line):
		stree.append(int(line))

if naive != stree:
	print "Error"
else:
	print "OK"

# vim: set ts=4 sts=4 sw=4 expandtab smartindent:
