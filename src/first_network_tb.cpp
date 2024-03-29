#include <iostream>
#include <fstream>
#include <random>
#include "../includes/neuron.h"
#include "../includes/defines.h"

int main() {
    std::vector<NeuronaLIF> capa1(NUM_NEURONAS_CAPA_1);
    std::vector<NeuronaLIF> capa2(NUM_NEURONAS_CAPA_2);
    std::vector<NeuronaLIF> capa3(NUM_NEURONAS_CAPA_3);

    // Abrir el archivo de salida
    std::ofstream archivoSalida("salida.txt");

    // Verificar si el archivo se abrió correctamente
    if (!archivoSalida.is_open()) {
        std::cerr << "Error al abrir el archivo de salida." << std::endl;
        return 1; // Salir con código de error
    }

    // Generacion de las señales de entrada aleatorias
    double corrientesEntrada[NUM_ENTRADAS];
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    for (int i=0; i<NUM_ITER; i++){
        archivoSalida << "Corrientes de entrada " << i+1 << ": " << std::endl;
        for (int j = 0; j < NUM_ENTRADAS; j++) {
            double randomValue = dis(gen); // Genera un valor aleatorio entre 0 y 1
            corrientesEntrada[j] = (randomValue <= PROB_ENTRADA_SPIKE) ? POTEN_NO_SPIKE : POTEN_SPIKE; // Mapea el valor a 0 o 2
            archivoSalida << "Entrada neurona " << j+1 << ":" << corrientesEntrada[j] << std::endl;
        }

        //Capa de entrada (1)
        simulate(&corrientesEntrada[0], NUM_ENTRADAS/NUM_NEURONAS_CAPA_1, &capa1[0]);
        simulate(&corrientesEntrada[1], NUM_ENTRADAS/NUM_NEURONAS_CAPA_1, &capa1[1]);

        archivoSalida << "\nRESULTADOS CAPA 1: " << std::endl;
        archivoSalida << "Neurona 1" << std::endl;
        archivoSalida << "Potencial de la membrana de la neurona: " << capa1[0].getPotencialMembrana() << std::endl;
        archivoSalida << "Potencial de salida de la neurona: " << capa1[0].getPotencialSalida() << std::endl;
        archivoSalida << "Neurona 2" << std::endl;
        archivoSalida << "Potencial de la membrana de la neurona: " << capa1[1].getPotencialMembrana() << std::endl;
        archivoSalida << "Potencial de salida de la neurona: " << capa1[1].getPotencialSalida() << std::endl;

        // Empaquetamiento salidas capa 1
        double corrientesCapa1[2] = { capa1[0].getPotencialSalida(), capa1[1].getPotencialSalida() };

        //Capa intermedia 1 (2)
        simulate(corrientesCapa1, NUM_NEURONAS_CAPA_1, &capa2[0]);
        simulate(corrientesCapa1, NUM_NEURONAS_CAPA_1, &capa2[1]);
        simulate(corrientesCapa1, NUM_NEURONAS_CAPA_1, &capa2[2]);

        archivoSalida << "\nRESULTADOS CAPA 2: " << std::endl;
        archivoSalida << "Neurona 1" << std::endl;
        archivoSalida << "Potencial de la membrana de la neurona: " << capa2[0].getPotencialMembrana() << std::endl;
        archivoSalida << "Potencial de salida de la neurona: " << capa2[0].getPotencialSalida() << std::endl;
        archivoSalida << "Neurona 2" << std::endl;
        archivoSalida << "Potencial de la membrana de la neurona: " << capa2[1].getPotencialMembrana() << std::endl;
        archivoSalida << "Potencial de salida de la neurona: " << capa2[1].getPotencialSalida() << std::endl;
        archivoSalida << "Neurona 3" << std::endl;
        archivoSalida << "Potencial de la membrana de la neurona: " << capa2[2].getPotencialMembrana() << std::endl;
        archivoSalida << "Potencial de salida de la neurona: " << capa2[2].getPotencialSalida() << std::endl;

        // Empaquetamiento salidas capa 2
        double corrientesCapa2[3] = { capa2[0].getPotencialSalida(), capa2[1].getPotencialSalida(), capa2[2].getPotencialSalida()};

        //Capa de salida (3)
        simulate(corrientesCapa2, NUM_NEURONAS_CAPA_2, &capa3[0]);

        archivoSalida << "\nRESULTADOS CAPA 3: " << std::endl;
        archivoSalida << "Neurona 1" << std::endl;
        archivoSalida << "Potencial de la membrana de la neurona: " << capa3[0].getPotencialMembrana() << std::endl;
        archivoSalida << "Potencial de salida de la neurona: " << capa3[0].getPotencialSalida() << "\n" << std::endl;
    }
    // Cerrar el archivo de salida
    archivoSalida.close();

    return 0;
}
