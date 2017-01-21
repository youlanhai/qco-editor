# -*- coding: utf-8 -*-
import sys
import os
import shutil
import re
from argparse import ArgumentParser

re_lib = re.compile(r".*(plugin-\w*|editor|cocos2d).dylib")

MODULE_PATH = os.path.dirname(os.path.abspath(__file__))

def list_dependency(lib):
	filename = "dependency.txt"
	os.system("otool -L %s > %s" % (lib, filename))

	lines = None
	with open(filename, "r") as f:
		lines = f.readlines()
	os.remove(filename)

	lines.pop(0)
	denpends = []
	for line in lines:
		line = line.strip()
		if len(line) != 0 and (line[0] not in "/@" or "libcocos2d." in line or "libluacocos2d." in line):
			# line = xxx.dylib (...)
			libname = line.split('(')[0].strip()
			denpends.append(libname)

	return denpends

def replace_dependency(lib_path, dst_prefix, libraries=None):
	print "\nresolve:", lib_path
	depends = list_dependency(lib_path)

	for depend_path in depends:
		depend_name = os.path.basename(depend_path)
		parts = depend_name.split('.')
		newname = parts[0] + "." + parts[-1]

		if libraries is None or newname in libraries:
			newname = os.path.join(dst_prefix, newname)
			print depend_name, "->", newname
			os.system("install_name_tool -change %s %s %s" % (depend_path, newname, lib_path))

def copy_libs(lib_path, dst_path, dst_prefix):
	print "copy libs:", lib_path

	libraries = os.listdir(lib_path)
	for fname in libraries:
		if not re_lib.match(fname): continue

		src_full_path = os.path.join(lib_path, fname)
		dst_full_path = os.path.join(dst_path, fname)

		print "copy:", fname
		shutil.copy(src_full_path, dst_full_path)

def resolve_file(file_path, lib_path, dst_prefix):
	replace_dependency(file_path, dst_prefix)

def resolve_app(app_name, prefix, build_mode):
	app_path = os.path.join(os.path.dirname(MODULE_PATH), "bin", build_mode, "mac")
	exe_dir = os.path.join(app_path, app_name + ".app", "Contents/MacOS")

	lib_path = app_path
	copy_libs(lib_path, exe_dir, "@executable_path")

	lib_path = os.path.join(os.path.dirname(MODULE_PATH), "cocos2d-x-" + build_mode, "lib")
	copy_libs(lib_path, exe_dir, "@executable_path")

	libraries = os.listdir(exe_dir)
	for fname in libraries:
		if os.path.splitext(fname)[1] in ("", ".dylib"):
			fpath = os.path.join(exe_dir, fname)
			replace_dependency(fpath, prefix, libraries)

# mac平台，lib依赖项的路径如果是相对路径，使用本工具处理。绝对路径使用macdeployqt处理。
def main():
	parser = ArgumentParser(description="resolve library dependency")
	parser.add_argument("-m", action="store", dest="mode", help="app|lib. lib use for a single plugin library.")
	parser.add_argument("-prefix", action="store", dest="prefix", default="@executable_path", help="library prefix.")
	parser.add_argument("-build-mode", action="store", dest="build_mode", default="debug", help="debug|release. the default value is 'debug'.")
	parser.add_argument("-app-name", action="store", dest="app_name", help="app name. mode=app")
	parser.add_argument("-lib", action="store", dest="lib_path", help="library file path. mode=lib")

	option = parser.parse_args()
	if option.mode == "app":
		if option.app_name is None:
			print "invalie app_name"
			return
		
		resolve_app(option.app_name, option.prefix, option.build_mode)

	elif option.mode == "lib":
		if option.lib_path is None:
			print "invalid lib_path"
			return
		replace_dependency(option.lib_path, option.prefix)
	else:
		print "invalid mode."
		parser.print_help()

if __name__ == "__main__":
	main()
