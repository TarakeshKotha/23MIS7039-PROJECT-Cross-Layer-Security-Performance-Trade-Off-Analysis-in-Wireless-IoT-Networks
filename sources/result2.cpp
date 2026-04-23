#include <fstream>
#include <cstdlib>

int main() {
    std::ofstream data("pdr.dat");
    double pdr[] = {0.72,0.74,0.75,0.78,0.76,0.73};

    for(int i=0;i<6;i++)
        data << i+1 << " " << pdr[i] << std::endl;

    data.close();

    std::ofstream plot("pdr.plt");
    plot << "set terminal png\n";
    plot << "set output 'figures/pdr_graph.png'\n";
    plot << "set title 'PDR Analysis'\n";
    plot << "set xlabel 'Flow ID'\n";
    plot << "set ylabel 'PDR'\n";
    plot << "plot 'pdr.dat' with linespoints\n";
    plot.close();

    system("gnuplot pdr.plt");
}