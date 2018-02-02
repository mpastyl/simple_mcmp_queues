import numpy as np
import matplotlib.pyplot as plt


Dict = {}
threads = [1,2,4,8,16,32]

with open("log.txt",'rb') as file:
	for line in file:
		contention = line.split(' ')[0]
		algo = line.split(' ')[1]
		t = int(line.split(' ')[2])
		thr = float(line.split(' ')[4])
		
		if Dict.has_key((contention,algo,t)):
			Dict[(contention,algo,t)].append(thr)
		else:
			Dict[(contention,algo,t)] = [thr]

#average across runs
for key in Dict:
	print key
	Dict[key] = sum(Dict[key])/len(Dict[key])

#high contention
lock_based = []
lock_free = []
for t in threads:
	lock_based.append( Dict[('high','lock_based',t)])
	lock_free.append( Dict[('high','lock_free',t)])

plt.plot(threads,lock_based,'r*-',label = "Lock-based")
plt.plot(threads,lock_free,'b*-', label = "Lock-free")
plt.ylabel("Throughput (Mops/sec)")
plt.xticks(threads)
plt.xlabel("Threads")
lgd = plt.legend()
plt.savefig("High contention",bbox_extra_artists=(lgd,), bbox_inches = "tight")
plt.title("High contention")
plt.show()


#low contention
lock_based = []
lock_free = []
for t in threads:
	lock_based.append( Dict[('low','lock_based',t)])
	lock_free.append( Dict[('low','lock_free',t)])

plt.plot(threads,lock_based,'r*-',label = "Lock-based")
plt.plot(threads,lock_free,'b*-', label = "Lock-free")
plt.ylabel("Throughput (Mops/sec)")
plt.xticks(threads)
plt.xlabel("Threads")
lgd = plt.legend()
plt.savefig("Low contention",bbox_extra_artists=(lgd,), bbox_inches = "tight")
plt.title("Low contention")
plt.show()

