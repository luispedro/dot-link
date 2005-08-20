#!/usr/bin/env python
import sys
nbytes=int(sys.argv[1])

if len(sys.argv) > 2:
	f=file(sys.argv[2])
else:
	f=sys.stdin

sys.stdout.write(f.read(nbytes))

