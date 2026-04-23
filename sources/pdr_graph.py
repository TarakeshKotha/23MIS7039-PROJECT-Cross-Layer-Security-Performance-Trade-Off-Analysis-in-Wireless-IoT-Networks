flows = [1,2,3,4,5,6]
pdr = [0.72, 0.74, 0.75, 0.78, 0.76, 0.73]

plt.figure()
plt.plot(flows, pdr, marker='o')
plt.title("Packet Delivery Ratio (Proposed Model)")
plt.xlabel("Flow ID")
plt.ylabel("PDR")
plt.grid()
plt.savefig("pdr_graph.png")
plt.show()