#include "../headers/snn.h"

template <size_t N, size_t M>
void init_matrix (double (&matrix)[N][M], double valor) {
    for (int i=0; i<N; i++){
        for (int j=0; j<M; j++){
            matriz[i][j] = valor;
        }
    }
}

int winner_takes_all(NeuronaLIF* capa, int numNeurons) {
    //Entraos a la funcion sabiendo que una neurona ha disparado
    double maxPotential = -1.0;
    int winnerIndex = -1;

    // Encontrar la neurona con el mayor potencial de membrana
    for (int i = 0; i < numNeurons; ++i) {
        if (capa[i].getPotencialMembrana() > maxPotential) {
            maxPotential = capa[i].getPotencialMembrana();
            winnerIndex = i;
        }
    }

    // Inhibir las otras neuronas y ponerlas en estado de espera
    for (int i = 0; i < numNeurons; ++i) {
        if (i != winnerIndex) {
            capa[i].setPotencialSalida(POTEN_NO_SPIKE);
            capa[i].setPotencialMembrana(capa[i].getPotencialReposo());
            capa[i].setEstado(EstadoNeurona::ENESPERA);
        }
    }

    return winnerIndex;
}

void update_weight(double* weight, double trace) {
    double LTPW = exp(-(*weight - W_INIT));
    double LTPX = exp(trace - A);
    double LTDW = -exp(*weight - W_INIT);
    double LTDX = exp(1 - trace - A);

    double LTP = LTPW * LTPX;
    double LTD = LTDW * LTDX;

    *weight += LEARNING_RATE * (LTP + LTD);
}

template <size_t N, size_t M>
void aplicar_STDP(NeuronaLIF* capaPre, int numPre, NeuronaLIF* capaPost, int numPost, int winnerIndex, double (&pesos)[N][M], double (&trazas)[N][M]) {
    if (winnerIndex != -1) {
        for (int i = 0; i < numPre; ++i) {
            // Reiniciar la traza si la neurona pre-sináptica dispara
            if (capaPre[i].getPotencialSalida() > POTEN_NO_SPIKE) {
                trazas[i][winnerIndex] = RESET_TRAZA;
            } else {
                // Decaer las trazas si la neurona pre-sináptica no dispara
                trazas[i][winnerIndex] *= exp(-DT / TAU_PLUS);
            }

            // Actualizar los pesos entre la neurona pre-sináptica y la ganadora post-sináptica
            update_weight(pesos[i][winnerIndex], trazas[i][winnerIndex]);
        }
    }
}




extern bool disparoCapa1;
extern bool disparoCapa2;
extern bool disparoCapa3;

int simulate_SNN () {
    NeuronaLIF capaEntrada[NUM_NEURONAS_CAPA_ENTRADA];
    NeuronaLIF capa2[NUM_NEURONAS_CAPA_2];
    NeuronaLIF capaSalida[NUM_NEURONAS_CAPA_SALIDA];

    double corrientescapaEntrada[NUM_NEURONAS_CAPA_ENTRADA];
    double corrientesCapa2[NUM_NEURONAS_CAPA_2];

    int winnerIndex;

    //------------------------- Creacion e inicializacion de matrices de pesos -------------------------//

    double weightsCapa1_2[NUM_NEURONAS_CAPA_ENTRADA][NUM_NEURONAS_CAPA_2];
    double weightsCapa2_3[NUM_NEURONAS_CAPA_2][NUM_NEURONAS_CAPA_SALIDA];

    init_matrix(weightsCapa1_2,W_INIT);
    init_matrix(weightsCapa2_3,W_INIT);

    //------------------------- Creacion e nicializacion de matrices de trazas -------------------------//

    double trazasCapa1_2[NUM_NEURONAS_CAPA_ENTRADA][NUM_NEURONAS_CAPA_2];
    double trazasCapa2_3[NUM_NEURONAS_CAPA_2][NUM_NEURONAS_CAPA_SALIDA];

    init_matrix(trazasCapa1_2,TRAZA_INIT);
    init_matrix(trazasCapa2_3,TRAZA_INIT);

    //------------------------- Generacion de las señales de entrada -------------------------//

    double corrientesEntrada[NUM_ENTRADAS];
    for (int j = 0; j < NUM_ENTRADAS; j++){
        corrientesEntrada[j]=POTEN_SPIKE;
    }

    //------------------------- Bucle principal de iteraciones de la red -------------------------//

    for (int i=0; i<NUM_ITER; i++){
        std::cout << "Corrientes de entrada " << i+1 << ":" << std::endl;
        for (int j = 0; j < NUM_ENTRADAS; j++) {
            //double randomValue = dis(gen); // Genera un valor aleatorio entre 0 y 1
            //corrientesEntrada[j] = (randomValue <= PROB_ENTRADA_SPIKE) ? POTEN_NO_SPIKE : POTEN_SPIKE; // Mapea el valor a 0 o 2
            std::cout << "Entrada neurona " << j+1 << ": " << corrientesEntrada[j] << std::endl;
        }

        //------------------------- Capa 1 (ENTRADA) -------------------------//
        for (int k=0; k<NUM_NEURONAS_CAPA_ENTRADA;k++) {
            simulate(&corrientesEntrada[k], NUM_ENTRADAS/NUM_NEURONAS_CAPA_ENTRADA, &capaEntrada[k], &disparoCapa1);
            corrientescapaEntrada[k] = capaEntrada[k].getPotencialSalida();
        }

        std::cout << "\nRESULTADOS CAPA 1: " << std::endl;
        for (int k=0;k<NUM_NEURONAS_CAPA_ENTRADA;k++){
            std::cout << "Neurona " << k+1 << std::endl;
            std::cout << "Potencial de la membrana de la neurona: " << capaEntrada[k].getPotencialMembrana() << std::endl;
            std::cout << "Potencial de salida de la neurona: " << capaEntrada[k].getPotencialSalida() << std::endl;
        }

        //Tras la capa de entrada no se aplica STDP

       //------------------------- Capa 2 (INTERMEDIA 1) -------------------------//

        for (int k=0; k<NUM_NEURONAS_CAPA_2;k++) {
            simulate(corrientescapaEntrada, NUM_NEURONAS_CAPA_ENTRADA, &capa2[k], &disparoCapa2);
            corrientesCapa2[k] = capa2[k].getPotencialSalida();
        }

        std::cout << "\nRESULTADOS CAPA 2: " << std::endl;
        for (int k=0;k<NUM_NEURONAS_CAPA_2;k++){
            std::cout << "Neurona " << k+1 << std::endl;
            std::cout << "Potencial de la membrana de la neurona: " << capa2[k].getPotencialMembrana() << std::endl;
            std::cout << "Potencial de salida de la neurona: " << capa2[k].getPotencialSalida() << std::endl;
        }

        if (disparoCapa2) {
            winnerIndex = winner_takes_all(capa2,NUM_NEURONAS_CAPA_2);
            aplicar_STDP(capaEntrada,NUM_NEURONAS_CAPA_ENTRADA,capa2,NUM_NEURONAS_CAPA_2,winnerIndex,weightsCapa1_2,trazasCapa1_2);
        }
        
        //------------------------- Capa 3 (SALIDA) -------------------------//

        for (int k=0; k<NUM_NEURONAS_CAPA_SALIDA;k++) {
            simulate(corrientesCapa2, NUM_NEURONAS_CAPA_2, &capaSalida[k], &disparoCapa3);
        }

        std::cout << "\nRESULTADOS CAPA 3: " << std::endl;
        for (int k=0;k<NUM_NEURONAS_CAPA_SALIDA;k++){
            std::cout << "Neurona " << k+1 << std::endl;
            std::cout << "Potencial de la membrana de la neurona: " << capaSalida[k].getPotencialMembrana() << std::endl;
            std::cout << "Potencial de salida de la neurona: " << capaSalida[k].getPotencialSalida() << std::endl;
        }

        if (disparoCapa3) {
            winnerIndex = winner_takes_all(capaSalida,NUM_NEURONAS_CAPA_SALIDA);
            aplicar_STDP(capaEntrada,NUM_NEURONAS_CAPA_ENTRADA,capa2,NUM_NEURONAS_CAPA_2,winnerIndex,weightsCapa1_2,trazasCapa1_2);
        }
    }

    return 0;
}