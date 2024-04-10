#include "../headers/snn.h"

int simulate_SNN () {
    NeuronaLIF capa1[NUM_NEURONAS_CAPA_1];
    NeuronaLIF capa2[NUM_NEURONAS_CAPA_2];
    NeuronaLIF capa3[NUM_NEURONAS_CAPA_3];

    // Generacion de las señales de entrada aleatorias
    double corrientesEntrada[NUM_ENTRADAS];
    for (int j = 0; j < NUM_ENTRADAS; j++){
        corrientesEntrada[j]=2.0;
    }

    /*// Abrir el archivo de salida
    std::ofstream archivoSalida("salida.txt");

    // Verificar si el archivo se abrió correctamente
    if (!archivoSalida.is_open()) {
        std::cerr << "Error al abrir el archivo de salida." << std::endl;
        return 1; // Salir con código de error
    }*/

    for (int i=0; i<NUM_ITER; i++){
        std::cout << "Corrientes de entrada " << i+1 << ":" << std::endl;
        for (int j = 0; j < NUM_ENTRADAS; j++) {
            //double randomValue = dis(gen); // Genera un valor aleatorio entre 0 y 1
            //corrientesEntrada[j] = (randomValue <= PROB_ENTRADA_SPIKE) ? POTEN_NO_SPIKE : POTEN_SPIKE; // Mapea el valor a 0 o 2
            std::cout << "Entrada neurona " << j+1 << ": " << corrientesEntrada[j] << std::endl;
        }

        //Capa de entrada (1)
        simulate(&corrientesEntrada[0], NUM_ENTRADAS/NUM_NEURONAS_CAPA_1, &capa1[0]);
        simulate(&corrientesEntrada[1], NUM_ENTRADAS/NUM_NEURONAS_CAPA_1, &capa1[1]);

        std::cout << "\nRESULTADOS CAPA 1: " << std::endl;
        std::cout << "Neurona 1" << std::endl;
        std::cout << "Potencial de la membrana de la neurona: " << capa1[0].getPotencialMembrana() << std::endl;
        std::cout << "Potencial de salida de la neurona: " << capa1[0].getPotencialSalida() << std::endl;
        std::cout << "Neurona 2" << std::endl;
        std::cout << "Potencial de la membrana de la neurona: " << capa1[1].getPotencialMembrana() << std::endl;
        std::cout << "Potencial de salida de la neurona: " << capa1[1].getPotencialSalida() << std::endl;

        // Empaquetamiento salidas capa 1
        double corrientesCapa1[2] = { capa1[0].getPotencialSalida(), capa1[1].getPotencialSalida() };

        //Capa intermedia 1 (2)
        simulate(corrientesCapa1, NUM_NEURONAS_CAPA_1, &capa2[0]);
        simulate(corrientesCapa1, NUM_NEURONAS_CAPA_1, &capa2[1]);
        simulate(corrientesCapa1, NUM_NEURONAS_CAPA_1, &capa2[2]);

        std::cout << "\nRESULTADOS CAPA 2: " << std::endl;
        std::cout << "Neurona 1" << std::endl;
        std::cout << "Potencial de la membrana de la neurona: " << capa2[0].getPotencialMembrana() << std::endl;
        std::cout << "Potencial de salida de la neurona: " << capa2[0].getPotencialSalida() << std::endl;
        std::cout << "Neurona 2" << std::endl;
        std::cout << "Potencial de la membrana de la neurona: " << capa2[1].getPotencialMembrana() << std::endl;
        std::cout << "Potencial de salida de la neurona: " << capa2[1].getPotencialSalida() << std::endl;
        std::cout << "Neurona 3" << std::endl;
        std::cout << "Potencial de la membrana de la neurona: " << capa2[2].getPotencialMembrana() << std::endl;
        std::cout << "Potencial de salida de la neurona: " << capa2[2].getPotencialSalida() << std::endl;

        // Empaquetamiento salidas capa 2
        double corrientesCapa2[3] = { capa2[0].getPotencialSalida(), capa2[1].getPotencialSalida(), capa2[2].getPotencialSalida()};

        //Capa de salida (3)
        simulate(corrientesCapa2, NUM_NEURONAS_CAPA_2, &capa3[0]);

        std::cout << "\nRESULTADOS CAPA 3: " << std::endl;
        std::cout << "Neurona 1" << std::endl;
        std::cout << "Potencial de la membrana de la neurona: " << capa3[0].getPotencialMembrana() << std::endl;
        std::cout << "Potencial de salida de la neurona: " << capa3[0].getPotencialSalida() << "\n" << std::endl;
    }
    // Cerrar el archivo de salida
    //archivoSalida.close();

    return 0;
}