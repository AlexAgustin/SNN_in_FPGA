#include "../headers/snn.h"

int simulate_SNN () {
    NeuronaLIF capaEntrada[NUM_NEURONAS_CAPA_ENTRADA];
    NeuronaLIF capa2[NUM_NEURONAS_CAPA_2];
    NeuronaLIF capaSalida[NUM_NEURONAS_CAPA_SALIDA];

    double corrientescapaEntrada[NUM_NEURONAS_CAPA_ENTRADA];
    double corrientesCapa2[NUM_NEURONAS_CAPA_2];

    // Generacion de las señales de entrada aleatorias
    double corrientesEntrada[NUM_ENTRADAS];
    for (int j = 0; j < NUM_ENTRADAS; j++){
        corrientesEntrada[j]=POTEN_SPIKE;
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
        for (int k=0; k<NUM_NEURONAS_CAPA_ENTRADA;k++) {
            simulate(&corrientesEntrada[k], NUM_ENTRADAS/NUM_NEURONAS_CAPA_ENTRADA, &capaEntrada[k]);
            corrientescapaEntrada[k] = capaEntrada[k].getPotencialSalida();
        }

        std::cout << "\nRESULTADOS CAPA 1: " << std::endl;
        for (int k=0;k<NUM_NEURONAS_CAPA_ENTRADA;k++){
            std::cout << "Neurona " << k+1 << std::endl;
            std::cout << "Potencial de la membrana de la neurona: " << capaEntrada[k].getPotencialMembrana() << std::endl;
            std::cout << "Potencial de salida de la neurona: " << capaEntrada[k].getPotencialSalida() << std::endl;
        }

        //Capa intermedia 1 (2)
        for (int k=0; k<NUM_NEURONAS_CAPA_2;k++) {
            simulate(corrientescapaEntrada, NUM_NEURONAS_CAPA_ENTRADA, &capa2[k]);
            corrientesCapa2[k] = capa2[k].getPotencialSalida();
        }

        std::cout << "\nRESULTADOS CAPA 2: " << std::endl;
        for (int k=0;k<NUM_NEURONAS_CAPA_2;k++){
            std::cout << "Neurona " << k+1 << std::endl;
            std::cout << "Potencial de la membrana de la neurona: " << capa2[k].getPotencialMembrana() << std::endl;
            std::cout << "Potencial de salida de la neurona: " << capa2[k].getPotencialSalida() << std::endl;
        }

        //Capa de salida (3)
        for (int k=0; k<NUM_NEURONAS_CAPA_SALIDA;k++) {
            simulate(corrientesCapa2, NUM_NEURONAS_CAPA_2, &capaSalida[k]);
        }

        std::cout << "\nRESULTADOS CAPA 3: " << std::endl;
        for (int k=0;k<NUM_NEURONAS_CAPA_SALIDA;k++){
            std::cout << "Neurona " << k+1 << std::endl;
            std::cout << "Potencial de la membrana de la neurona: " << capaSalida[k].getPotencialMembrana() << std::endl;
            std::cout << "Potencial de salida de la neurona: " << capaSalida[k].getPotencialSalida() << std::endl;
        }
    }
    // Cerrar el archivo de salida
    //archivoSalida.close();

    return 0;
}