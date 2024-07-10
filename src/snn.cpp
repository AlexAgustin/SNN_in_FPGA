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
            std::cout << "Neurona Actual: " << capa[i].getPotencialMembrana() << "Max: " << maxPotential << std::endl;
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
void aplicar_STDP(int winnerIndex, double (&weights)[N][M], double (&trazas)[N][M]) {
    if (winnerIndex != -1) {

        //update_traces(capaPre,numPre,numPost,trazas);

        // Actualizar los weights entre la neurona pre-sináptica y la ganadora post-sináptica
        update_weight(weights[winnerIndex], trazas[winnerIndex]);
        
    }
}




bool disparoCapa1;
bool disparoCapa2;
bool disparoCapa3;

void simulate_SNN () {
    std::cout << "1====>" << std::endl;
    NeuronaLIF capaEntrada[NUM_NEURONAS_CAPA_ENTRADA];
    NeuronaLIF capa2[NUM_NEURONAS_CAPA_2];
    NeuronaLIF capaSalida[NUM_NEURONAS_CAPA_SALIDA];

    double outputCapaEntrada[NUM_NEURONAS_CAPA_ENTRADA];
    double outputCapa2[NUM_NEURONAS_CAPA_2];
    double outputCapaSalida[NUM_NEURONAS_CAPA_SALIDA];

    int winnerIndex;

    //------------------------- Creacion e inicializacion de matrices de weights -------------------------//

    double weightsCapaEntrada[1][NUM_NEURONAS_CAPA_ENTRADA];
    double weightsCapa1_2[NUM_NEURONAS_CAPA_2][NUM_NEURONAS_CAPA_ENTRADA];
    double weightsCapa2_3[NUM_NEURONAS_CAPA_SALIDA][NUM_NEURONAS_CAPA_2];

    init_matrix(weightsCapaEntrada,1);
    init_matrix(weightsCapa1_2,W_INIT);
    init_matrix(weightsCapa2_3,W_INIT);

    //------------------------- Creacion e nicializacion de matrices de trazas -------------------------//

    //double trazasCapaEntry[NUM_NEURONAS_CAPA_ENTRADA][NUM_ENTRADAS];
    double trazasCapa1_2[NUM_NEURONAS_CAPA_2][NUM_NEURONAS_CAPA_ENTRADA];
    double trazasCapa2_3[NUM_NEURONAS_CAPA_SALIDA][NUM_NEURONAS_CAPA_2];

    //init_matrix(trazasCapaEntry,TRAZA_INIT);
    init_matrix(trazasCapa1_2,TRAZA_INIT);
    init_matrix(trazasCapa2_3,TRAZA_INIT);

    //------------------------- Generacion de las señales de entrada + Aprendizaje -------------------------//

    double input[NUM_NEURONAS_CAPA_ENTRADA];

    std::string root_directory = "C:\\Users\\alexm\\Documents\\Cuarto\\TFG\\MINST_processed_100";

    int event_count = 1,max = 0, mod_timestamp = 0;
    int neurona_corresp=-1;

    ProcessedEvent event;

    try {
        for (const auto& entry : fs::directory_iterator(root_directory)) {
            if (fs::is_directory(entry.status())) {
                std::cout << entry.path().string() << std::endl;

                for (const auto& sub_entry : fs::directory_iterator(entry.path())) {
                    if (fs::is_directory(sub_entry.status())) {
                        std::cout << sub_entry.path().string() << std::endl;

                        for (const auto& file_entry : fs::directory_iterator(sub_entry.path())) {
                            if (fs::is_regular_file(file_entry.status())) {
                                std::cout << file_entry.path().string() << std::endl;

                                std::ifstream infile(file_entry.path().string(), std::ios::binary);
                                if (!infile) {
                                    break;
                                }

                                while (event_count) {
                                    event_count = 0;
                                    disparoCapa1=false;
                                    disparoCapa2=false;
                                    disparoCapa3=false;
                                    for (int j = 0; j < NUM_ITER; j++){
                                        input[j] = POTEN_NO_SPIKE;
                                    }

                                    while (infile.read(reinterpret_cast<char*>(&event.timestamp), sizeof(event.timestamp))) {
                                        infile.read(reinterpret_cast<char*>(&event.x), sizeof(event.x));
                                        infile.read(reinterpret_cast<char*>(&event.y), sizeof(event.y));
                                        infile.read(reinterpret_cast<char*>(&event.polarity), sizeof(event.polarity));

                                        /*std::cout << "Timestamp: " << event.timestamp
                                                << ", X: " << event.x
                                                << ", Y: " << event.y
                                                << ", Polarity: " << event.polarity << std::endl;*/

                                        if (static_cast<int>(event.timestamp / 1000) !=mod_timestamp){
                                            //std::cout << "Evento: " << event_count << " tiene timestamp " << event.timestamp << " y el mod es " << mod_timestamp << std::endl;
                                            break;
                                        }

                                        neurona_corresp = (static_cast<int>(event.x) % 16) * (static_cast<int>(event.y) % 16) + (static_cast<int>(event.x) % 16) + 16*16 * event.polarity;

                                        if (neurona_corresp > max) max = neurona_corresp;
                                        //std::cout << "Corresponde a la neurona: " << neurona_corresp << std::endl;
                                        input[neurona_corresp]=THRESHOLD;

                                        event_count++;
                                    } //En este punto se han procesado todos los eventos que ocurren en la misma milesima de segundo y se han establecido spikes en las neuronas que coresponden a las posiciones en las que han ocurrido esos eventos
                                    
                                    //Incremento de la variable que selecciona la milesima de segundo
                                    mod_timestamp++;

                                    //Simulacion de la capa de entrada
                                    for (int i=0; i<NUM_NEURONAS_CAPA_ENTRADA;i++) {
                                        simulate_entry(input[i], &capaEntrada[i], &disparoCapa1);
                                        outputCapaEntrada[i] = capaEntrada[i].getPotencialSalida();
                                    }

                                    //Actualizacion de las trazas sinapticas de la capa de entrada
                                    update_traces(capaEntrada, trazasCapa1_2);

                                    //En la capa de entrada no se aplica STDP ya que solo replica los spikes que recibe

                                    //Simulacion capa 2
                                    for (int i=0; i<NUM_NEURONAS_CAPA_2;i++) {
                                        simulate(outputCapaEntrada, NUM_NEURONAS_CAPA_ENTRADA, &capa2[i], &disparoCapa2, weightsCapa1_2[i]);
                                    }

                                    //STDP de capa 2
                                    if (disparoCapa2) {
                                        winnerIndex = winner_takes_all(capa2);
                                        aplicar_STDP(winnerIndex,weightsCapa1_2,trazasCapa1_2);
                                    }

                                    for (int j=0; j<NUM_NEURONAS_CAPA_2;j++) {
                                        outputCapa2[j] = capa2[j].getPotencialSalida();
                                    }

                                    std::cout << "OUTPUT DE LAS NEURONAS DE LA CAPA 2:" << std::endl;
                                    for (int i = 0; i < NUM_NEURONAS_CAPA_2; ++i) {
                                        std::cout << outputCapa2[i] << " ";
                                    }
                                    std::cout << std::endl;

                                    //Actualizacion de trazas capa 2
                                    update_traces(capa2, trazasCapa2_3);

                                    //Simulacion capa 3
                                    for (int i=0; i<NUM_NEURONAS_CAPA_SALIDA;i++) {
                                        simulate(outputCapa2, NUM_NEURONAS_CAPA_2, &capaSalida[i], &disparoCapa3, weightsCapa2_3[i]);
                                    }

                                    //STDP de capa 3
                                    if (disparoCapa3) {
                                        winnerIndex = winner_takes_all(capaSalida);
                                        aplicar_STDP(winnerIndex,weightsCapa2_3,trazasCapa2_3);
                                    }

                                    for (int i=0; i<NUM_NEURONAS_CAPA_SALIDA;i++) {
                                        outputCapaSalida[i] = capaSalida[i].getPotencialSalida();
                                    }

                                    std::cout << "OUTPUT DE LAS NEURONAS DE LA ULTIMA CAPA:" << std::endl;
                                    for (int i = 0; i < NUM_NEURONAS_CAPA_SALIDA; ++i) {
                                        std::cout << outputCapaSalida[i] << " ";
                                    }
                                    std::cout << "\n" << std::endl;
                                }

                                event_count=-1;
                                mod_timestamp=0;
                            }
                        }
                        std::cout << "ID de neurona maximo es: " << max << std::endl;
                    }
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    //------------------------- Bucle principal de iteraciones de la red -------------------------//

    /*for (int i=0; i<NUM_ITER; i++){
        disparoCapa1=false;
        disparoCapa2=false;

        
        std::cout << "\n\nCorrientes de entrada " << i+1 << ":" << std::endl;
        std::cout << "Input 1: " << input1[i] << std::endl;
        std::cout << "Input 2: " << input2[i] << std::endl;
        for (int j = 0; j < NUM_ENTRADAS; j++) {
            //double randomValue = dis(gen); // Genera un valor aleatorio entre 0 y 1
            //corrientesEntrada[j] = (randomValue <= PROB_ENTRADA_SPIKE) ? POTEN_NO_SPIKE : POTEN_SPIKE; // Mapea el valor a 0 o 2
            
        }

        //------------------------- Capa 1 (ENTRADA) -------------------------//
        
        simulate_entry(input1[i],&capaEntrada[0],&disparoCapa1);
        simulate_entry(input2[i],&capaEntrada[1],&disparoCapa1);

        outputCapaEntrada[0] = capaEntrada[0].getPotencialSalida();
        outputCapaEntrada[1] = capaEntrada[1].getPotencialSalida();

        update_traces(capaEntrada, trazasCapa1_2);
        
        for (int k=0; k<NUM_NEURONAS_CAPA_ENTRADA;k++) {
            simulate(&corrientesEntrada[k], NUM_ENTRADAS/NUM_NEURONAS_CAPA_ENTRADA, &capaEntrada[k], &disparoCapa1, weightsCapaEntry[k]);
            outputCapaEntrada[k] = capaEntrada[k].getPotencialSalida();
        }

        std::cout << "\nRESULTADOS CAPA ENTRADA: " << std::endl;
        for (int k=0;k<NUM_NEURONAS_CAPA_ENTRADA;k++){
            std::cout << "Neurona " << k+1 << std::endl;
            std::cout << "Potencial de la membrana de la neurona: " << capaEntrada[k].getPotencialMembrana() << std::endl;
            std::cout << "Potencial de salida de la neurona: " << capaEntrada[k].getPotencialSalida() << std::endl;
        }

        //Tras la capa de entrada no se aplica STDP

       //------------------------- Capa 2 (INTERMEDIA 1) -------------------------//

        for (int k=0; k<NUM_NEURONAS_CAPA_2;k++) {
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
        }
        
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
            aplicar_STDP(winnerIndex,weightsCapa1_2,trazasCapa1_2);
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
    }*/

}