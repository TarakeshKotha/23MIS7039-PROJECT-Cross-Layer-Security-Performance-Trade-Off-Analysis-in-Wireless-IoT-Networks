
import matplotlib.pyplot as plt
plr = [28, 26, 25, 23, 24, 27]
flows = [1,2,3,4,5,6]

plt.figure()
plt.plot(flows, plr, marker='o')
plt.title("Packet Loss Analysis")
plt.xlabel("Flow ID")
plt.ylabel("Packet Loss (%)")
plt.grid()
plt.savefig("packet_loss_graph.png")
plt.show()