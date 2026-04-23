#include <fstream>
#include <cstdlib>

int main() {
    std::ofstream data("throughput.dat");
    int flows[] = {1,2,3,4,5,6};
    double thr[] = {55,58,60,62,59,61};

    for(int i=0;i<6;i++)
        data << flows[i] << " " << thr[i] << std::endl;

    data.close();

    std::ofstream plot("throughput.plt");
    plot << "set terminal png\n";
    plot << "set output 'figures/throughput_graph.png'\n";
    plot << "set title 'Throughput Analysis'\n";
    plot << "set xlabel 'Flow ID'\n";
    plot << "set ylabel 'Throughput (kbps)'\n";
    plot << "plot 'throughput.dat' with linespoints\n";
    plot.close();

    system("gnuplot throughput.plt");
}