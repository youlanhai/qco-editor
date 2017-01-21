# -*- coding: utf-8 -*-

import os
import sys

EXTS = set((".h", ".hpp", ".hxx", ".c", ".cpp", ".cxx", ))

class Collector(object):

	def __init__(self):
		super(Collector, self).__init__()
		self.nfiles = 0
		self.nlines = 0
		self.nblankline = 0

	def collect(self, path):

		for root, dirs, files in os.walk(path):
			print root
			for fname in files:
				if os.path.splitext(fname)[-1] in EXTS:
					fpath = os.path.join(root, fname)
					self.collectFile(fpath)

		return

	def collectFile(self, fpath):
		self.nfiles += 1
		with open(fpath, "rb") as f:
			for line in f.xreadlines():
				self.nlines += 1
				if len(line.strip()) == 0:
					self.nblankline += 1

		return

def main():
	if len(sys.argv) < 2:
		return

	path = sys.argv[1]

	co = Collector()
	co.collect(path)

	print "--------"
	print "file:", co.nfiles
	print "lines:", co.nlines - co.nblankline
	print "total lines:", co.nlines
	print "blank lines:", co.nblankline

if __name__ == "__main__":
	main()
