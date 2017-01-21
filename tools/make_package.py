
import os
import sys
import shutil
import re

MODULE_PATH = os.path.abspath(os.path.dirname(__file__))

SVN_URL = "https://192.168.0.11:8443/svn/XProject/trunk/Dev/Editor"
PACKAGE_PATH = os.path.join(MODULE_PATH, "editor_package")
APP_NAME = "CocosEditor-release"
SVN_ARG = "--username zhoubao --password zb123456"

def package_win32():
	dst_win32_path = os.path.join(PACKAGE_PATH, "win32")
	if not os.path.isdir(dst_win32_path):
		os.mkdir(dst_win32_path)

	app_path = os.path.join("win32", APP_NAME + ".exe")

	dst_path = os.path.join(PACKAGE_PATH, app_path)
	if os.path.isfile(dst_path):
		print "\nremove:", dst_path
		os.remove(dst_path)

	src_path = os.path.join(MODULE_PATH, app_path)
	print "\ncopy: ", src_path
	shutil.copy(src_path, dst_path)

def package_mac():
	platform = "mac"

	app_path = os.path.join(platform, APP_NAME + ".app")

	dst_path = os.path.join(PACKAGE_PATH, app_path)
	if os.path.isdir(dst_path):
		print "\nremove: ", dst_path
		shutil.rmtree(dst_path)
	
	src_path = os.path.join(MODULE_PATH, app_path)
	print "\ncopy:", src_path
	shutil.copytree(src_path, dst_path)

	QT_COMPILER = os.getenv("QT_COMPILER")
	if QT_COMPILER is None:
		raise RuntimeError, "system varianble 'QT_COMPILER' was not found"

	MACDEPLOYQT = os.path.join(QT_COMPILER, "clang_64/bin/macdeployqt")

	print "\ndependency: ", dst_path
	os.system("%s %s" % (MACDEPLOYQT, dst_path))

def update():
	print "update:", PACKAGE_PATH

	if not os.path.isdir(PACKAGE_PATH):
		cmd = """svn co "%s" "%s" %s""" % (SVN_URL, PACKAGE_PATH, SVN_ARG, )
	else:
		cmd = """svn up "%s" %s""" % (PACKAGE_PATH, SVN_ARG, )
	os.system(cmd)

def copy_assets():
	dirs = ("res", "language", )

	for path in dirs:
		dst_res_path = os.path.join(PACKAGE_PATH, path)
		if os.path.isdir(dst_res_path):
			print "\nremove:", dst_res_path
			shutil.rmtree(dst_res_path)

		src_res_path = os.path.join(MODULE_PATH, path)
		print "\ncopy: ", src_res_path
		shutil.copytree(src_res_path, dst_res_path)

	return

def add_unversiond_files():
	tmp_file = "svn_tmp.txt"
	os.system("svn status %s > %s" % (PACKAGE_PATH, tmp_file, ))

	handle = open(tmp_file)
	for line in handle.xreadlines():
		splits = re.split(r"\?\s+", line)
		if len(splits) >= 2:
			filename = splits[1]
			os.system("""svn add "%s" """ % (filename, ))

	handle.close()
	os.remove(tmp_file)

def commit():
	add_unversiond_files()

	cmd = """svn commit -m "update editor" "%s" %s""" % (PACKAGE_PATH, SVN_ARG, )
	os.system(cmd)

def main():
	update()
	copy_assets()

	if sys.platform == "win32":
		package_win32()
	elif sys.platform == "darwin":
		package_mac()
	else:
		raise RuntimeError, "unknown operator system."

	#commit()

if __name__ == "__main__":
	main()
