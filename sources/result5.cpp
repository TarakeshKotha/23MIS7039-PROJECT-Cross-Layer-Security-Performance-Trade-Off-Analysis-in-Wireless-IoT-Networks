#include <fstream>
#include <cstdlib>

int main() {
    std::ofstream data("throughput_distribution.dat");

    int nodes[] = {1,2,3,4,5,6,7,8,9,10};
    double thr[] = {18,18,19,18,17,18,18,19,18,18};

    for(int i=0;i<10;i++)
        data << nodes[i] << " " << thr[i] << std::endl;

    data.close();

    std::ofstream plot("throughput_distribution.plt");

    plot << "set terminal png size 800,600\n";
    plot << "set output 'figures/throughput_distribution.png'\n";
    plot << "set title 'Throughput Distribution Across Nodes'\n";
    plot << "set xlabel 'Node ID'\n";
    plot << "set ylabel 'Throughput (kbps)'\n";
    plot << "set style data histograms\n";
    plot << "set style fill solid\n";
    plot << "plot 'throughput_distribution.dat' using 2:xtic(1) title 'Throughput'\n";

    plot.close();

    system("gnuplot throughput_distribution.plt");
}