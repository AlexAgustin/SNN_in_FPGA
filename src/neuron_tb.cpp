#include "neuron.h"

int main() {
    NeuronaLIF n;

    double corrientesEntrada[NUM_ENTRADAS];
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    for (int i=0; i<NUM_ITER; i++)
    {
        std::cout << "Entradas numero " << i+1 << ":" << std::endl;
        for (int j=0; j<NUM_ENTRADAS; j++) {
            double randomValue = dis(gen); // Genera un valor aleatorio entre 0 y 1
            corrientesEntrada[j] = (randomValue <= 0.3) ? 0.0 : 2.0; // Mapea el valor a 0 o 2
            std::cout << corrientesEntrada[j] << std::endl;
        }


        //neurona.simular(corrientesEntrada);
        simulate(corrientesEntrada, &n);
    }

    return 0;
}
