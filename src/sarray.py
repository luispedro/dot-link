input=file('file')

string=input.readline().strip() + '$'

naive=range(len(string))
def sufcmp(a,b):
	if a == b:
		return 0
	while a < len(string) and b < len(string) and string[a] == string[b]:
		a += 1
		b += 1
	if a == len(string):
		return 1
	if b == len(string):
		return -1
	if string[a] < string[b]:
		return -1
	return 1
naive.sort(sufcmp)
	
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
