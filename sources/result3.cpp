#include <fstream>
#include <cstdlib>

int main() {
    std::ofstream data("plr.dat");
    double plr[] = {28,26,25,23,24,27};

    for(int i=0;i<6;i++)
        data << i+1 << " " << plr[i] << std::endl;

    data.close();

    std::ofstream plot("plr.plt");
    plot << "set terminal png\n";
    plot << "set output 'figures/packet_loss_graph.png'\n";
    plot << "set title 'Packet Loss'\n";
    plot << "set xlabel 'Flow ID'\n";
    plot << "set ylabel 'Loss (%)'\n";
    plot << "plot 'plr.dat' with linespoints\n";
    plot.close();

    system("gnuplot plr.plt");
}