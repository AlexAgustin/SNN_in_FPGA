#include "../headers/snn.h"

template <size_t N, size_t M>
void init_matrix (double (&matriz)[N][M], double valor) {
    for (int i=0; i<N; i++){
        for (int j=0; j<M; j++){
            matriz[i][j] = valor;
        }
    }
}

template <size_t N>
int winner_takes_all(NeuronaLIF (&capa)[N]) {
    //Entraos a la funcion sabiendo que una neurona ha disparado
    double maxPotential = -1.0;
    int winnerIndex = -1;

    // Encontrar la neurona con el mayor potencial de membrana
    for (int i = 0; i < N; ++i) {
        if (capa[i].getPotencialMembrana() > maxPotential) {
            maxPotential = capa[i].getPotencialMembrana();
            winnerIndex = i;
        }
    }

    // Inhibir las otras neuronas y ponerlas en estado de espera
    for (int i = 0; i < N; ++i) {
        if (i != winnerIndex) {
            capa[i].setPotencialSalida(POTEN_NO_SPIKE);
            capa[i].setPotencialMembrana(capa[i].getPotencialReposo());
            capa[i].setEstado(EstadoNeurona::ENESPERA);
        }
    }

    return winnerIndex;
}

template <size_t N, size_t M>
void update_traces(NeuronaLIF* capaPre, int numPre, int numPost, double (&trazas)[N][M]) {
    for (int i=0; i<numPre; i++) {
        if (capaPre[i].getPotencialSalida() == POTEN_SPIKE) {
            for (int j=0; j<numPost; j++) {
                trazas[j][i] += ALPHA;
            }
        } else {
            for (int j=0; j<numPost; j++) {
                trazas[j][i] *= exp(-DT / TAU_PLUS);
            }
        }
    }
}

template <size_t N>
void normalize_traces (double (&traces)[N]) {
    double max=0.0;

    for (int i=0; i<N; i++) {
        if (traces[i]>max) {
            max = traces[i];
        }
    }

    for (int i=0; i<N; i++) {
        traces[i] /= max;
    }
}

template <size_t N>
void update_weight(double (&weights)[N], double (&traces)[N]) {


    double LTPW;
    double LTPX;
    double LTDW;
    double LTDX;

    double LTP;
    double LTD;

    normalize_traces(traces);

    for (int i = 0; i < N; ++i) {

        LTPW = exp(-(weights[i] - W_INIT));
        LTPX = exp(traces[i] - A);
        LTDW = -exp(weights[i] - W_INIT);
        LTDX = exp(1 - traces[i] - A);

        LTP = LTPW * LTPX;
        LTD = LTDW * LTDX;

        weights[i] += LEARNING_RATE * (LTP + LTD);
    }
}

template <size_t N, size_t M>
void aplicar_STDP(NeuronaLIF* capaPre, NeuronaLIF* capaPost, int winnerIndex, double (&weights)[N][M], double (&trazas)[N][M]) {
    if (winnerIndex != -1) {

        //update_traces(capaPre,numPre,numPost,trazas);

        // Actualizar los weights entre la neurona pre-sináptica y la ganadora post-sináptica
        update_weight(weights[winnerIndex], trazas[winnerIndex]);
        
    }
}




bool disparoCapa1;
bool disparoCapa2;
//extern bool disparoCapa3;

int simulate_SNN () {
    NeuronaLIF capaEntrada[NUM_NEURONAS_CAPA_ENTRADA];
    //NeuronaLIF capa2[NUM_NEURONAS_CAPA_2];
    NeuronaLIF capaSalida[NUM_NEURONAS_CAPA_SALIDA];

    double outputCapaEntrada[NUM_NEURONAS_CAPA_ENTRADA];
    //double corrientesCapa2[NUM_NEURONAS_CAPA_2];

    int winnerIndex;

    //------------------------- Creacion e inicializacion de matrices de weights -------------------------//

    double weightsCapaEntry[NUM_NEURONAS_CAPA_ENTRADA][NUM_ENTRADAS];
    double weightsCapa1_2[NUM_NEURONAS_CAPA_SALIDA][NUM_NEURONAS_CAPA_ENTRADA];
    //double weightsCapa2_3[NUM_NEURONAS_CAPA_2][NUM_NEURONAS_CAPA_SALIDA];

    init_matrix(weightsCapaEntry,W_INIT);
    init_matrix(weightsCapa1_2,W_INIT);
    //init_matrix(weightsCapa2_3,W_INIT);

    //------------------------- Creacion e nicializacion de matrices de trazas -------------------------//

    double trazasCapaEntry[NUM_NEURONAS_CAPA_ENTRADA][NUM_ENTRADAS];
    double trazasCapa1_2[NUM_NEURONAS_CAPA_SALIDA][NUM_NEURONAS_CAPA_ENTRADA];
    //double trazasCapa2_3[NUM_NEURONAS_CAPA_2][NUM_NEURONAS_CAPA_SALIDA];

    init_matrix(trazasCapaEntry,TRAZA_INIT);
    init_matrix(trazasCapa1_2,TRAZA_INIT);
    //init_matrix(trazasCapa2_3,TRAZA_INIT);

    //------------------------- Generacion de las señales de entrada -------------------------//

    double input1[NUM_ITER];
    double input2[NUM_ITER];
    for (int j = 0; j < NUM_ITER; j++){
        if (j < 15) {
            input1[j] = THRESHOLD;
            input2[j] = POTEN_NO_SPIKE;
        } else if (j < 25) {
            input1[j] = POTEN_NO_SPIKE;
            input2[j] = POTEN_NO_SPIKE;
        } else {
            input1[j] = POTEN_NO_SPIKE;
            input2[j] = THRESHOLD;
        }
    }

    //------------------------- Bucle principal de iteraciones de la red -------------------------//

    for (int i=0; i<NUM_ITER; i++){
        disparoCapa1=false;
        disparoCapa2=false;

        
        std::cout << "\n\nCorrientes de entrada " << i+1 << ":" << std::endl;
        std::cout << "Input 1: " << input1[i] << std::endl;
        std::cout << "Input 2: " << input2[i] << std::endl;
        /*for (int j = 0; j < NUM_ENTRADAS; j++) {
            //double randomValue = dis(gen); // Genera un valor aleatorio entre 0 y 1
            //corrientesEntrada[j] = (randomValue <= PROB_ENTRADA_SPIKE) ? POTEN_NO_SPIKE : POTEN_SPIKE; // Mapea el valor a 0 o 2
            
        }*/

        //------------------------- Capa 1 (ENTRADA) -------------------------//
        
        simulate_entry(input1[i],&capaEntrada[0],&disparoCapa1);
        simulate_entry(input2[i],&capaEntrada[1],&disparoCapa1);

        outputCapaEntrada[0] = capaEntrada[0].getPotencialSalida();
        outputCapaEntrada[1] = capaEntrada[1].getPotencialSalida();

        update_traces(capaEntrada, NUM_NEURONAS_CAPA_ENTRADA, NUM_NEURONAS_CAPA_SALIDA, trazasCapa1_2);
        
        /*for (int k=0; k<NUM_NEURONAS_CAPA_ENTRADA;k++) {
            simulate(&corrientesEntrada[k], NUM_ENTRADAS/NUM_NEURONAS_CAPA_ENTRADA, &capaEntrada[k], &disparoCapa1, weightsCapaEntry[k]);
            outputCapaEntrada[k] = capaEntrada[k].getPotencialSalida();
        }*/

        std::cout << "\nRESULTADOS CAPA ENTRADA: " << std::endl;
        for (int k=0;k<NUM_NEURONAS_CAPA_ENTRADA;k++){
            std::cout << "Neurona " << k+1 << std::endl;
            std::cout << "Potencial de la membrana de la neurona: " << capaEntrada[k].getPotencialMembrana() << std::endl;
            std::cout << "Potencial de salida de la neurona: " << capaEntrada[k].getPotencialSalida() << std::endl;
        }

        //Tras la capa de entrada no se aplica STDP

       //------------------------- Capa 2 (INTERMEDIA 1) -------------------------//

        /*for (int k=0; k<NUM_NEURONAS_CAPA_2;k++) {
            simulate(outputCapaEntrada, NUM_NEURONAS_CAPA_ENTRADA, &capa2[k], &disparoCapa2);
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
        }*/
        
        //------------------------- Capa 3 (SALIDA) -------------------------//

        for (int k=0; k<NUM_NEURONAS_CAPA_SALIDA;k++) {
            simulate(outputCapaEntrada, NUM_NEURONAS_CAPA_ENTRADA, &capaSalida[k], &disparoCapa2, weightsCapa1_2[k]);
        }

        std::cout << "\nRESULTADOS CAPA salida: " << std::endl;
        for (int k=0;k<NUM_NEURONAS_CAPA_SALIDA;k++){
            std::cout << "Neurona " << std::endl;
            std::cout << "Potencial de la membrana de la neurona: " << capaSalida[k].getPotencialMembrana() << std::endl;
            std::cout << "Potencial de salida de la neurona: " << capaSalida[k].getPotencialSalida() << std::endl;
        }

        if (disparoCapa2) {
            winnerIndex = winner_takes_all(capaSalida);
            aplicar_STDP(capaEntrada,capaSalida,winnerIndex,weightsCapa1_2,trazasCapa1_2);
        }

        for (int k=0;k<NUM_NEURONAS_CAPA_SALIDA;k++){
            for (int l=0; l<NUM_NEURONAS_CAPA_ENTRADA; l++){
                std::cout << "\n" << k+1 << std::endl;
                std::cout << "weights " << k+1 << std::endl;
                std::cout << "[" << k << "] [" << l << "] = " << weightsCapa1_2[k][l] << std::endl;
            }
        }

        for (int k=0;k<NUM_NEURONAS_CAPA_SALIDA;k++){
            for (int l=0; l<NUM_NEURONAS_CAPA_ENTRADA; l++){
                std::cout << "\n" << std::endl;
                std::cout << "traces " << k+1 << std::endl;
                std::cout << "[" << k << "] [" << l << "] = " << trazasCapa1_2[k][l] << std::endl;
            }
        }
    }

    return 0;
}