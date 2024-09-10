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
            //registro_file << "Neurona Actual: " << capa[i].getPotencialMembrana() << "Max: " << maxPotential << std::endl;
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

        //registro_file << weights[i] << " " << traces[i] << std::endl;

        LTPW = exp(-(weights[i] - W_INIT));
        LTPX = exp(traces[i] - A);
        LTDW = -exp(weights[i] - W_INIT);
        LTDX = exp(1 - traces[i] - A);

        //registro_file << "LTPW: " << LTPW << " LTPX: " << LTPX << " LTDW: " << LTDW << " LTDX: " << LTDX << std::endl;

        LTP = LTPW * LTPX;
        LTD = LTDW * LTDX;

        //registro_file << "LTP: " << LTP << " LTD: " << LTD << std::endl;

        weights[i] += LEARNING_RATE * (LTP + LTD);

        //registro_file << weights[i] << std::endl;
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
    NeuronaLIF capaEntrada[NUM_NEURONAS_CAPA_ENTRADA];
    NeuronaLIF capa2[NUM_NEURONAS_CAPA_2];
    NeuronaLIF capaSalida[NUM_NEURONAS_CAPA_SALIDA];

    double outputCapaEntrada[NUM_NEURONAS_CAPA_ENTRADA];
    double outputCapa2[NUM_NEURONAS_CAPA_2];
    double outputCapaSalida[NUM_NEURONAS_CAPA_SALIDA];

    int winnerIndex;

    //------------------------- Creacion e inicializacion de matrices de weights -------------------------//

    double weightsCapaEntrada = 1;
    double weightsCapa1_2[NUM_NEURONAS_CAPA_2][NUM_NEURONAS_CAPA_ENTRADA];
    double weightsCapa2_3[NUM_NEURONAS_CAPA_SALIDA][NUM_NEURONAS_CAPA_2];

    //init_matrix(weightsCapaEntrada,1);
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

    std::string root_directory = "C:\\Users\\alexm\\Documents\\Cuarto\\TFG\\MNIST_processed";

    int event_count = 1,max = 0, mod_timestamp = 0;
    int neurona_corresp=-1;

    ProcessedEvent event;

    std::ofstream registro_file("registro_toda_BD(2).txt");

    // Verifica si se abrió correctamente
    if (!registro_file.is_open()) {
        std::cerr << "Error al abrir el archivo de registros." << std::endl;
        return;
    }

    try {
        for (const auto& entry : fs::directory_iterator(root_directory)) {
            if (fs::is_directory(entry.status())) {
                registro_file << entry.path().string() << std::endl;

                for (const auto& sub_entry : fs::directory_iterator(entry.path())) {
                    if (fs::is_directory(sub_entry.status())) {
                        registro_file << sub_entry.path().string() << std::endl;

                        for (const auto& file_entry : fs::directory_iterator(sub_entry.path())) {
                            if (fs::is_regular_file(file_entry.status())) {
                                registro_file << file_entry.path().string() << std::endl;

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

                                        /*registro_file << "Timestamp: " << event.timestamp
                                                << ", X: " << event.x
                                                << ", Y: " << event.y
                                                << ", Polarity: " << event.polarity << std::endl;*/

                                        if (static_cast<int>(event.timestamp / 1000) !=mod_timestamp){
                                            //registro_file << "Evento: " << event_count << " tiene timestamp " << event.timestamp << " y el mod es " << mod_timestamp << std::endl;
                                            break;
                                        }

                                        neurona_corresp = (static_cast<int>(event.x) % GRID) * (static_cast<int>(event.y) % GRID) + (static_cast<int>(event.x) % GRID) + GRID*GRID * event.polarity;

                                        if (neurona_corresp > max) max = neurona_corresp;
                                        //registro_file << "Corresponde a la neurona: " << neurona_corresp << std::endl;
                                        input[neurona_corresp]+=POTEN_SPIKE;

                                        event_count++;
                                    } //En este punto se han procesado todos los eventos que ocurren en la misma milesima de segundo y se han establecido spikes en las neuronas que coresponden a las posiciones en las que han ocurrido esos eventos
                                    
                                    /*registro_file << "INPUT DE LA RED:" << std::endl;
                                    for (int i = 0; i < NUM_NEURONAS_CAPA_ENTRADA; i++) {
                                        registro_file << input[i];
                                    }
                                    registro_file << std::endl;*/

                                    //Incremento de la variable que selecciona la milesima de segundo
                                    mod_timestamp++;

                                    //Simulacion de la capa de entrada
                                    for (int i=0; i<NUM_NEURONAS_CAPA_ENTRADA;i++) {
                                        simulate(&input[i], 1, &capaEntrada[i], &disparoCapa1, &weightsCapaEntrada);
                                        outputCapaEntrada[i] = capaEntrada[i].getPotencialSalida();
                                    }

                                    /*registro_file << "ENTRADA CAPA OUTPUT:" << std::endl;
                                    for (int i = 0; i < NUM_NEURONAS_CAPA_ENTRADA; i++) {
                                        registro_file << outputCapaEntrada[i] << " ";
                                    }
                                    registro_file << std::endl;*/

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

                                    /*registro_file << "SEGUNDA CAPA OUTPUT:" << std::endl;
                                    for (int i = 0; i < NUM_NEURONAS_CAPA_2; i++) {
                                        registro_file << outputCapa2[i] << " ";
                                    }
                                    registro_file << std::endl;*/

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

                                    registro_file << "SALIDA DE LA RED:" << std::endl;
                                    for (int i = 0; i < NUM_NEURONAS_CAPA_SALIDA; i++) {
                                        registro_file << outputCapaSalida[i] << " ";
                                    }
                                    registro_file << "\n" << std::endl;
                                }

                                event_count=-1;
                                mod_timestamp=0;
                            }

                            /*registro_file << "Pesos capa 1_2" << std::endl;
                            for (int i=0; i < NUM_NEURONAS_CAPA_ENTRADA; i++) {
                                for (int j=0; j < NUM_NEURONAS_CAPA_2; j++) {
                                    registro_file << weightsCapa1_2[j][i] << " ";
                                }
                                registro_file << std::endl;
                            }
                            registro_file << "\n";*/

                        }
                        //registro_file << "ID de neurona maximo es: " << max << std::endl;
                    }
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    registro_file << "Pesos capa 1_2" << std::endl;
    for (int i=0; i < NUM_NEURONAS_CAPA_ENTRADA; i++) {
        for (int j=0; j < NUM_NEURONAS_CAPA_2; j++) {
            registro_file << weightsCapa1_2[j][i] << " ";
        }
        registro_file << std::endl;
    }
    registro_file << "\n";

    registro_file << "Pesos capa 2_3" << std::endl;
    for (int i=0; i < NUM_NEURONAS_CAPA_2; i++) {
        for (int j=0; j < NUM_NEURONAS_CAPA_SALIDA; j++) {
            registro_file << weightsCapa2_3[j][i] << " ";
        }
        registro_file << std::endl;
    }
    registro_file << "\n";

    // Finalmente, cierra el archivo cuando ya no sea necesario.
    registro_file.close();

}