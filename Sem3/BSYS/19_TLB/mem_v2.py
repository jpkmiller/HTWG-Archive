import subprocess

def main():
	times = []
	repeat = 1000
	print("repeats for " + str(repeat) + " times")
	for x in xrange(0, repeat, 1):
		times.append(subprocess.call(["./tlb_mes_v2", str(x), "100"]))
main()
