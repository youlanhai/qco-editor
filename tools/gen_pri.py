
import os
import sys

def gen_pri(input_path, output_file_path):
	input_path = os.path.normpath(input_path)

	path_len = len(input_path) + 1

	hpps = []
	cpps = []
	for root, dirs, files in os.walk(input_path):
		for name in files:
			name = os.path.join(root[path_len:], name)
			name = name.replace("\\", '/')
			ext = os.path.splitext(name)[1]
			if ext == ".h":
				hpps.append(name)
			elif ext == ".cpp":
				cpps.append(name)

	ret = []

	ret.append("HEADERS +=\\\n")
	for name in hpps:
		ret.append("\t$$PWD/%s \\\n" % name)

	ret.append("\nSOURCES +=\\\n")
	for name in cpps:
		ret.append("\t$$PWD/%s \\\n" % name)

	content = "".join(ret)
	with open(output_file_path, "w") as f:
		f.write(content)

def main():
	if len(sys.argv) < 3:
		print "usage: python %s input_path output_file_path" % __file__
		return

	gen_pri(sys.argv[1], sys.argv[2])

if __name__ == "__main__":
	main()
