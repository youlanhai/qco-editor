import os
import sys

def main():
	if len(sys.argv) < 2:
		print "useage: test src_path"
		return

	src_path = sys.argv[1]
	files = os.listdir(src_path)
	for name in files:
		if os.path.splitext(name)[1] != ".layout": continue
		
		path = os.path.join(src_path, name)
		cmd = "./smartjson \"%s\"" % path
		print cmd
		os.system(cmd)

if __name__ == "__main__":
	main()

