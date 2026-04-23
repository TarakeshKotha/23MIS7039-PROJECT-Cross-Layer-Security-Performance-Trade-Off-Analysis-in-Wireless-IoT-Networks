#include <fstream>
#include <cstdlib>

int main() {
    std::ofstream data("txrx.dat");
    int tx[] = {180,185,190,200,195,188};
    int rx[] = {140,145,150,160,155,148};

    for(int i=0;i<6;i++)
        data << i+1 << " " << tx[i] << " " << rx[i] << std::endl;

    data.close();

    std::ofstream plot("txrx.plt");
    plot << "set terminal png\n";
    plot << "set output 'figures/tx_rx_graph.png'\n";
    plot << "set title 'TX vs RX'\n";
    plot << "plot 'txrx.dat' using 1:2 with lines title 'TX', \\\n";
    plot << "'txrx.dat' using 1:3 with lines title 'RX'\n";
    plot.close();

    system("gnuplot txrx.plt");
}