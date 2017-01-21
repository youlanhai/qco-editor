# -*- coding: utf-8 -*-

import sys
import os


def get_file_ext(name):
	pos = name.find('.')
	if pos < 0: return None

	return name[ pos+1: ]


def format_path_name(name):
	if len(name) == 0: return name

	new_name = name.replace('\\', '/')
	if new_name[-1] != '/': new_name += '/'

	return new_name


def if_file_ext_match(name, filters=None):
	if filters is None: return True

	ext = get_file_ext(name)
	if ext is None: return False

	return ext in filters


def prefix_old2new(name, prefix_old, prefix_new):
	len_old = len(prefix_old)
	if len(name) < len_old: return None
	if name[ :len_old ] != prefix_old: return None

	return prefix_new + name[ len_old: ]


def collect_replace_prefix(prefix_old, prefix_new, path, filters=None):
	namemap = {}
	
	files = os.listdir(path)
	for name in files:
		if not if_file_ext_match(name, filters): continue

		new_name = prefix_old2new(name, prefix_old, prefix_new)
		if new_name is None: continue

		namemap[name] = new_name

	return namemap


INT_a = ord('a')
INT_z = ord('z')
INT_A = ord('A')
INT_Z = ord('Z')
INT_A2a = INT_a - INT_A

def big2small(name):

	ret = ""

	#only add a underline when last character is bwteen 'a' and 'z'
	canAddLine = False

	for ch in name:
		int_ch = ord(ch)
		if INT_A <= int_ch <= INT_Z:
			if canAddLine: ret += '_'

			ch = chr(int_ch + INT_A2a)
			canAddLine = False

		elif INT_a <= int_ch <= INT_z:
			canAddLine = True

		else: #maybe it's a digital or underline.
			canAddLine = False

		ret += ch

	return ret

def collect_big2small(path, filters=None):
	namemap = {}

	files = os.listdir(path)
	for name in files:
		if if_file_ext_match(name, filters):
			new_name = big2small(name)
			if new_name != name: namemap[name] = new_name

	return namemap

##################################################
###
##################################################

def do_replace_in_file(path, namemap, filters):
	print "---do_replace_in_file", path, len(namemap)

	if len(namemap) == 0: return

	files = os.listdir(path)
	for name in files:
		if not if_file_ext_match(name, filters): continue

		fname = path + name
		print "replace content: ", fname

		f = open(fname, "rb")
		content = f.read()
		f.close()

		for k, v in namemap.iteritems():
			content = content.replace(k, v)

		f = open(fname, "wb")
		f.write(content)
		f.close()


def do_replace_in_name(path, namemap):
	print "---do_replace_in_name", path, len(namemap)

	for k, v in namemap.iteritems():
		os.system( "git mv -f %s %s" % (path + k, path + v) )


def do_big2small(path, filters=None):
	namemap = collect_big2small(path, filters)

	#1st. rename file
	do_replace_in_name(path, namemap)

	#2st. replace content
	do_replace_in_file(path, namemap, filters)

def do_replace_prefix(prefix_old, prefix_new, path, filters=None):
	namemap = collect_replace_prefix(prefix_old, prefix_new, path, filters)

	do_replace_in_name(path, namemap)
	do_replace_in_file(path, namemap, filters)

##################################################
### upgrade
##################################################

def upgrade_editor(path, filters):
	do_replace_prefix("Editor", "", path, filters)
	do_big2small(path, filters)

def upgrade_gui(path, filters):
	do_replace_prefix("UI", "", path, filters)
	do_big2small(path, filters)

def upgrade_loaders(path, filters):
	do_replace_prefix("CC", "", path, filters)
	do_big2small(path, filters)

##################################################
### main
##################################################

usage = """
usage:
python renamefiles.py path
"""

def main():
	if len(sys.argv) < 2:
		print usage
	else:
		path = format_path_name( sys.argv[1] )


		filters = ("h", "hpp", "c", "cpp", "pri")
		#upgrade_editor(path, filters)
		#upgrade_loaders(path, filters)
		upgrade_gui(path, filters)

if __name__ == "__main__":
	main()



