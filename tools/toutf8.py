# -*- coding: utf-8 -*-

import sys
import os

FILTERS = set((".h", ".cpp", ".hpp"))
BOM = "\xEF\xBB\xBF"
ENCODES = ("utf-8", "gb2312", "gbk")

def usage():
	print "python toutf8.py path"

def to_utf8(path, bom):
	source = None
	with open(path, "rb") as f:
		source = f.read()

	if len(source) > 3 and source[:3] == BOM:
		return False

	content = None
	for code in ENCODES:
		try:
			content = source.decode(code)
			break
		except:
			pass
	else:
		print "Failed to decode file", path
		return False

	content = content.encode("utf-8")
	if not bom and content == source:
		return False

	with open(path, "wb") as f:
		if bom: f.write(BOM)
		f.write(content)
	return True

def convert(path):
	total = 0
	converted = 0
	for root, dirs, files in os.walk(path):
		i = 0
		while i < len(dirs):
			if dirs[i][0] == '.':
				dirs.pop(i)
			else:
				i += 1

		for name in files:
			if os.path.splitext(name)[1] in FILTERS:
				total += 1
				full_path = os.path.join(root, name)
				if to_utf8(full_path, True):
					converted += 1

	print "total: %d, converted: %d" % (total, converted)
	return

def main():
	if len(sys.argv) <= 1:
		return usage()

	convert(sys.argv[1])

if __name__ == "__main__":
	main()
