#include <fstream>
#include <cstdlib>

int main() {
    std::ofstream data("reliability.dat");

    int flows[] = {1,2,3,4,5,6};
    double rel[] = {0.72,0.74,0.76,0.78,0.75,0.73};

    for(int i=0;i<6;i++)
        data << flows[i] << " " << rel[i] << std::endl;

    data.close();

    std::ofstream plot("reliability.plt");

    plot << "set terminal png size 800,600\n";
    plot << "set output 'figures/reliability_pdr.png'\n";
    plot << "set title 'Reliability Analysis'\n";
    plot << "set xlabel 'Flow ID'\n";
    plot << "set ylabel 'Reliability'\n";
    plot << "plot 'reliability.dat' with linespoints title 'Reliability'\n";

    plot.close();

    system("gnuplot reliability.plt");
}