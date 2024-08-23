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
            //std::cout << "Neurona Actual: " << capa[i].getPotencialMembrana() << "Max: " << maxPotential << std::endl;
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
void update_traces(NeuronaLIF* capaPre, double (&trazas)[N][M]) {
    for (int i=0; i<M; i++) {
        if (capaPre[i].getPotencialSalida() == POTEN_SPIKE) {
            for (int j=0; j<N; j++) {
                trazas[j][i] += ALPHA;
            }
        } else {
            for (int j=0; j<N; j++) {
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

        std::cout << weights[i] << " " << traces[i] << std::endl;

        LTPW = exp(-(weights[i] - W_INIT));
        LTPX = exp(traces[i] - A);
        LTDW = -exp(weights[i] - W_INIT);
        LTDX = exp(1 - traces[i] - A);

        std::cout << "LTPW: " << LTPW << " LTPX: " << LTPX << " LTDW: " << LTDW << " LTDX: " << LTDX << std::endl;

        LTP = LTPW * LTPX;
        LTD = LTDW * LTDX;

        std::cout << "LTP: " << LTP << " LTD: " << LTD << std::endl;

        weights[i] += LEARNING_RATE * (LTP + LTD);

        std::cout << weights[i] << std::endl;
    }
}

template <size_t N, size_t M>
void aplicar_STDP(int winnerIndex, double (&weights)[N][M], double (&trazas)[N][M]) {
    if (winnerIndex != -1) {

        //update_traces(capaPre,numPre,numPost,trazas);

        // Actualizar los weights entre la neurona pre-sináptica y la ganadora post-sináptica
        update_weight(weights[winnerIndex], trazas[winnerIndex]);
        
    }
}

int main() {
    NeuronaLIF capaEntrada[2];
    NeuronaLIF capaSalida[3];

    double inputnE1[500];
    double inputnE2[500];
    int i2=0;

    for (int i=0; i<500; i++) {
        i2 = i%6;
        switch (i2)
        {
        case 0:
            inputnE1[i]=2;
            inputnE2[i]=2;
            break;
        case 1:
            inputnE1[i]=2;
            inputnE2[i]=0;
            break;
        case 2:
            inputnE1[i]=2;
            inputnE2[i]=0;
            break;
        case 3:
            inputnE1[i]=2;
            inputnE2[i]=2;
            break;
        case 4:
            inputnE1[i]=0;
            inputnE2[i]=2;
            break;
        case 5:
            inputnE1[i]=0;
            inputnE2[i]=2;
            break;

        }
    }

    double outputCapaEntrada[2];

    double weightsEntrada = 1;
    double weightsCapa1_2[3][2];
    init_matrix(weightsCapa1_2,W_INIT);

    double trazasCapa1_2[3][2];
    init_matrix(trazasCapa1_2,TRAZA_INIT);

    bool disparo1;
    bool disparo2;
    int winnerIndex;

    for (int i=0; i<500; i++) {
        disparo1=false;
        disparo2=false;
        std::cout << "Inputs: nE1=" << inputnE1[i] << " nE2=" << inputnE2[i] << std::endl;

        simulate(&inputnE1[i], 1, &capaEntrada[0], &disparo1, &weightsEntrada);
        outputCapaEntrada[0] = capaEntrada[0].getPotencialSalida();
        simulate(&inputnE2[i], 1, &capaEntrada[1], &disparo1, &weightsEntrada);
        outputCapaEntrada[1] = capaEntrada[1].getPotencialSalida();

        std::cout << "Potencial de membrana capa entrada: \nnE1=" << capaEntrada[0].getPotencialMembrana() << " nE2=" << capaEntrada[1].getPotencialMembrana() << std::endl;
        std::cout << "Salidas capa entrada: \nnE1=" << outputCapaEntrada[0] << " nE2=" << outputCapaEntrada[1] << std::endl;

        update_traces(capaEntrada,trazasCapa1_2);

        std::cout << "Trazas capa1->2: \ntraza[0][0]=" << trazasCapa1_2[0][0] << " traza[0][1]=" << trazasCapa1_2[0][1] << " \ntraza[1][0]=" << trazasCapa1_2[1][0] << 
                                     " traza[1][1]=" << trazasCapa1_2[1][1] << " \ntraza[2][0]=" << trazasCapa1_2[2][0] << " traza[2][1]=" << trazasCapa1_2[2][1] << std::endl;

        simulate(outputCapaEntrada, 2, &capaSalida[0], &disparo2, weightsCapa1_2[0]);
        simulate(outputCapaEntrada, 2, &capaSalida[1], &disparo2, weightsCapa1_2[1]);
        simulate(outputCapaEntrada, 2, &capaSalida[2], &disparo2, weightsCapa1_2[2]);

        std::cout << "Potencial de membrana capa salida: \nnS1=" << capaSalida[0].getPotencialMembrana() << " nS2=" << capaSalida[1].getPotencialMembrana() << " nS3=" << capaSalida[2].getPotencialMembrana() << std::endl;

        if (disparo2) {
            winnerIndex = winner_takes_all(capaSalida);
            aplicar_STDP(winnerIndex,weightsCapa1_2,trazasCapa1_2);
        }
        std::cout << "Salidas capa salida: \nnS1=" << capaSalida[0].getPotencialSalida() << " nS2=" << capaSalida[1].getPotencialSalida() << " nS3=" << capaSalida[2].getPotencialSalida() << std::endl;

        std::cout << "Pesos capa1->2: \npeso[0][0]=" << weightsCapa1_2[0][0] << " peso[0][1]=" << weightsCapa1_2[0][1] << " \npeso[1][0]=" << weightsCapa1_2[1][0] << 
                                     " peso[1][1]=" << weightsCapa1_2[1][1] << " \npeso[2][0]=" << weightsCapa1_2[2][0] << " peso[2][1]=" << weightsCapa1_2[2][1] << "\n" << std::endl;
    }

    return 0;
}