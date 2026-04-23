import matplotlib.pyplot as plt
flows = [1,2,3,4,5,6]
throughput = [55, 58, 60, 62, 59, 61]

plt.figure()
plt.plot(flows, throughput, marker='o')
plt.title("Throughput Analysis (Proposed Model)")
plt.xlabel("Flow ID")
plt.ylabel("Throughput (kbps)")
plt.grid()
plt.savefig("throughput_graph.png")
plt.show()