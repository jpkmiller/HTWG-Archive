import subprocess

def main():
	times = []
	repeat = 100
	print("repeats for " + str(repeat) + " times")
	for x in xrange(0, 5000, 3):
		times.append(subprocess.call(["./tlb_mes", str(x), str(repeat)]))
main()
