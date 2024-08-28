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

        //std::cout << weights[i] << " " << traces[i] << std::endl;

        LTPW = exp(-(weights[i] - W_INIT));
        LTPX = exp(traces[i] - A);
        LTDW = -exp(weights[i] - W_INIT);
        LTDX = exp(1 - traces[i] - A);

        //std::cout << "LTPW: " << LTPW << " LTPX: " << LTPX << " LTDW: " << LTDW << " LTDX: " << LTDX << std::endl;

        LTP = LTPW * LTPX;
        LTD = LTDW * LTDX;

        //std::cout << "LTP: " << LTP << " LTD: " << LTD << std::endl;

        weights[i] += LEARNING_RATE * (LTP + LTD);

        //std::cout << weights[i] << std::endl;
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

int simulate_SNN () {
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

    //------------------------- Montaje de la Tarjeta SD -------------------------//
    FATFS fs;
    FRESULT res;
    
    res = f_mount(&fs, "", 1);
    if (res != FR_OK) {
        std::cout << "Error mounting SD card: " << res << "\n" << std::endl;
        return ERR_MOUNT_FAIL;
    }

    //------------------------- Inicializacion de la escritura en SD -------------------------//
    FIL resultados_file;
    UINT bw;
    char buff[1024];
    int offset = 0;

    res = f_open(&resultados_file, "resultados.txt", FA_WRITE | FA_CREATE_ALWAYS);
    if (res != FR_OK) {
        std::cout << "Error opening resultados file: " << res << "\n" << std::endl;
        return ERR_OPEN_RESULT_FILE;
    }

    //------------------------- Inicializacion de la lectura desde SD -------------------------//
    FIL entradas_file;
    UINT br;
    char file_path[256];

    double input[NUM_NEURONAS_CAPA_ENTRADA];
    int event_count = 1, max = 0, mod_timestamp = 0;
    int neurona_corresp = -1;
    ProcessedEvent event;

    // Recorre la base de dato MNIST
    for (int digit = 0; digit <= 9; digit++) {
        for (int scale = 4; scale <= 16; scale *= 2) {
            for (int file_num = 1; file_num <= 1000; file_num++) {
                sprintf(file_path, "MNIST_processed/processed_data%d/scale%d/mnist_%d_scale%d_%04d.bin",scale, scale, digit, scale, file_num);

                // Abre el archivo binario
                res = f_open(&entradas_file, file_path, FA_READ);
                if (res != FR_OK) {
                    sprintf(buff, "Error opening file: %s\n", file_path);
                    // Registra el error de lectura en el fichero de salida
                    res = f_write(&resultados_file,file_path,strlen(file_path),&bw);
                    if (res != FR_OK || bw != strlen(file_path)) {
                        f_close(&resultados_file);  // Se cierra fichero de resultados
                        f_close(&entradas_file);    // Se cierra fichero de entrada
                        f_mount(NULL, "", 1);       // Se desmonta sistema de archivos
                        return ERR_WRITE_FAIL_11;
                    }
                    continue; //Continuar con el siguiente fichero de entrada
                }

                // Registra la ruta del archivo en el fichero de salida
                res = f_write(&resultados_file,file_path,strlen(file_path),&bw);
                if (res != FR_OK || bw != strlen(file_path)) {
                    f_close(&resultados_file);  // Se cierra fichero de resultados
                    f_close(&entradas_file);    // Se cierra fichero de entrada
                    f_mount(NULL, "", 1);       // Se desmonta sistema de archivos
                    return ERR_WRITE_FAIL_1;
                }

                // Procesa el archivo binario por eventos
                // El programa sale del bucle cuando la cuenta de eventos se mantiene en 0 en una de las iteraciones
                while (event_count) {

                    // Reset de variables del entornos
                    event_count = 0;
                    disparoCapa1 = false;
                    disparoCapa2 = false;
                    disparoCapa3 = false;
                    for (int j = 0; j < NUM_ITER; j++) {
                        input[j] = POTEN_NO_SPIKE;
                    }

                    // Bucle en el que se leen y procesan lineas del binario (eventos)
                    while (f_read(&entradas_file, &event.timestamp, sizeof(event.timestamp), &br) == FR_OK && br == sizeof(event.timestamp)) {
                        f_read(&entradas_file, &event.x, sizeof(event.x), &br);
                        f_read(&entradas_file, &event.y, sizeof(event.y), &br);
                        f_read(&entradas_file, &event.polarity, sizeof(event.polarity), &br);

                        // Registro de la informacion del evento leido
                        sprintf(buff, "Timestamp: %f, X: %f, Y: %f, Polarity: %f", event.timestamp, event.x, event.y, event.polarity);
                        res = f_write(&resultados_file,buff,strlen(buff),&bw);
                        if (res != FR_OK || bw != strlen(buff)) {
                            f_close(&resultados_file);  // Se cierra fichero de resultados
                            f_close(&entradas_file);    // Se cierra fichero de entrada
                            f_mount(NULL, "", 1);       // Se desmonta sistema de archivos
                            return ERR_WRITE_FAIL_2;
                        }
                        
                        // Comprobacion de que el evento ocurre en la misma milesima de segundo
                        if (static_cast<int>(event.timestamp / 1000) != mod_timestamp) {
                            //Registro de la informacion del timestamp y la milesima de segundo en la que se esta trabajando (mod_timestamp)
                            sprintf(buff, "Evento: %d tiene timestamp %f y el mod es %d", event_count, event.timestamp, mod_timestamp);
                            res = f_write(&resultados_file,buff,strlen(buff),&bw);
                            if (res != FR_OK || bw != strlen(buff)) {
                                f_close(&resultados_file);  // Se cierra fichero de resultados
                                f_close(&entradas_file);    // Se cierra fichero de entrada
                                f_mount(NULL, "", 1);       // Se desmonta sistema de archivos
                                return ERR_WRITE_FAIL_3;
                            }
                            break;
                        }

                        // Calculo de la neurona a la que corresponde el evento que se esta tratando
                        neurona_corresp = (static_cast<int>(event.x) % GRID) * (static_cast<int>(event.y) % GRID) + (static_cast<int>(event.x) % GRID) + GRID * GRID * event.polarity;
                        // Registro de la neurona a la que corresponde el evento que se esta tratando
                        sprintf(buff, "Corresponde a la neurona: %d", neurona_corresp);
                        res = f_write(&resultados_file,buff,strlen(buff),&bw);
                        if (res != FR_OK || bw != strlen(buff)) {
                            f_close(&resultados_file);  // Se cierra fichero de resultados
                            f_close(&entradas_file);    // Se cierra fichero de entrada
                            f_mount(NULL, "", 1);       // Se desmonta sistema de archivos
                            return ERR_WRITE_FAIL_4;
                        }

                        // Incremento del potencial de entrada de la neurona correspondiente
                        input[neurona_corresp] += POTEN_SPIKE;

                        // Control del indice de neurona maximo para control de errores
                        if (neurona_corresp > max) max = neurona_corresp;
                    
                        // Incremento del numero de eventos tratados
                        event_count++;
                    }

                    // En este punto se han procesado todos los eventos que ocurren en la misma milesima de segundo 
                    // y se han establecido spikes en las neuronas que coresponden a las posiciones en las que han ocurrido esos eventos

                    //-------------------- Registrar entradas a la red  --------------------//
                    offset = 0;

                    // Se copia la cabecera al buffer
                    offset += sprintf(buff + offset, "INPUT DE LA RED:\n");

                    // Se itera sobre input y se escriben en el buffer
                    for (int i = 0; i < NUM_NEURONAS_CAPA_ENTRADA; i++) {
                        offset += sprintf(buff + offset, "%f ", input[i]);

                        // Si el buffer está casi lleno, es escribe en el archivo y se reinicia
                        if (offset > sizeof(buff) - 50) {  // Se deja algo de espacio para evitar desbordamientos
                            res = f_write(&resultados_file, buff, offset, &bw);
                            if (res != FR_OK || bw != offset) {
                                f_close(&resultados_file);  // Se cierra fichero de resultados
                                f_close(&entradas_file);    // Se cierra fichero de entrada
                                f_mount(NULL, "", 1);       // Se desmonta sistema de archivos
                                return ERR_WRITE_FAIL_6;  
                            }
                            offset = 0;  // Se reinicia el offset después de escribir
                        }
                    }

                    // Se escribe el resto de datos(si los hay)
                    if (offset > 0) {
                        offset += sprintf(buff + offset, "\n");  // Se añade una nueva línea al final
                        res = f_write(&resultados_file, buff, offset, &bw);
                        if (res != FR_OK || bw != offset) {
                            f_close(&resultados_file);  // Se cierra fichero de resultados
                            f_close(&entradas_file);    // Se cierra fichero de entrada
                            f_mount(NULL, "", 1);       // Se desmonta sistema de archivos
                            return ERR_WRITE_FAIL_7;
                        }   
                    }
                    //---------------------------------------------------------------------------------//

                    // Incremento de la variable que selecciona la milesima de segundo
                    mod_timestamp++;

                    // Simulación de la capa de entrada
                    for (int i = 0; i < NUM_NEURONAS_CAPA_ENTRADA; i++) {
                        simulate(&input[i], 1, &capaEntrada[i], &disparoCapa1, &weightsCapaEntrada);
                        outputCapaEntrada[i] = capaEntrada[i].getPotencialSalida();
                    }

                    //-------------------- Registrar salidas de la capa de entrada  --------------------//
                    offset = 0;

                    // Se copia la cabecera al buffer
                    offset += sprintf(buff + offset, "CAPA ENTRADA OUTPUT:\n");

                    // Iteramos sobre outputCapaEntrada y escribimos en el buffer
                    for (int i = 0; i < NUM_NEURONAS_CAPA_ENTRADA; i++) {
                        offset += sprintf(buff + offset, "%f ", outputCapaEntrada[i]);

                        // Si el buffer está casi lleno, lo escribimos en el archivo y lo reiniciamos
                        if (offset > sizeof(buff) - 50) {  // Deja algo de espacio para evitar desbordamientos
                            res = f_write(&resultados_file, buff, offset, &bw);
                            if (res != FR_OK || bw != offset) {
                                f_close(&resultados_file);  // Se cierra fichero de resultados
                                f_close(&entradas_file);    // Se cierra fichero de entrada
                                f_mount(NULL, "", 1);       // Se desmonta sistema de archivos
                                return ERR_WRITE_FAIL_6;  
                            }
                            offset = 0;  // Reinicia el offset después de escribir
                        }
                    }

                    // Escribir el resto de datos(si los hay)
                    if (offset > 0) {
                        offset += sprintf(buff + offset, "\n");  // Añadir nueva línea al final si hay datos pendientes
                        res = f_write(&resultados_file, buff, offset, &bw);
                        if (res != FR_OK || bw != offset) {
                            f_close(&resultados_file);  // Se cierra fichero de resultados
                            f_close(&entradas_file);    // Se cierra fichero de entrada
                            f_mount(NULL, "", 1);       // Se desmonta sistema de archivos
                            return ERR_WRITE_FAIL_7;
                        }   
                    }
                    //---------------------------------------------------------------------------------//


                    // Actualización de las trazas sinápticas de la capa de entrada
                    update_traces(capaEntrada, trazasCapa1_2);

                    // En la capa de entrada no se aplica STDP ya que solo replica los spikes que recibe

                    // Simulación capa 2
                    for (int i = 0; i < NUM_NEURONAS_CAPA_2; i++) {
                        simulate(outputCapaEntrada, NUM_NEURONAS_CAPA_ENTRADA, &capa2[i], &disparoCapa2, weightsCapa1_2[i]);
                    }

                    // STDP de capa 2
                    if (disparoCapa2) {
                        winnerIndex = winner_takes_all(capa2);
                        aplicar_STDP(winnerIndex, weightsCapa1_2, trazasCapa1_2);
                    }

                    for (int j = 0; j < NUM_NEURONAS_CAPA_2; j++) {
                        outputCapa2[j] = capa2[j].getPotencialSalida();
                    }

                    //-------------------- Registrar salidas de la capa 2  --------------------//
                    offset = 0;

                    // Se copia la cabecera al buffer
                    offset += sprintf(buff + offset, "SEGUNDA CAPA OUTPUT:\n");

                    // Iteramos sobre outputCapa2 y escribimos en el buffer
                    for (int i = 0; i < NUM_NEURONAS_CAPA_2; i++) {
                        offset += sprintf(buff + offset, "%f ", outputCapa2[i]);

                        // Si el buffer está casi lleno, lo escribimos en el archivo y lo reiniciamos
                        if (offset > sizeof(buff) - 50) {  // Deja algo de espacio para evitar desbordamientos
                            res = f_write(&resultados_file, buff, offset, &bw);
                            if (res != FR_OK || bw != offset) {
                                f_close(&resultados_file);  // Se cierra fichero de resultados
                                f_close(&entradas_file);    // Se cierra fichero de entrada
                                f_mount(NULL, "", 1);       // Se desmonta sistema de archivos
                                return ERR_WRITE_FAIL_8;  
                            }
                            offset = 0;  // Reinicia el offset después de escribir
                        }
                    }

                    // Escribir el resto de datos(si los hay)
                    if (offset > 0) {
                        offset += sprintf(buff + offset, "\n");  // Añadir nueva línea al final si hay datos pendientes
                        res = f_write(&resultados_file, buff, offset, &bw);
                        if (res != FR_OK || bw != offset) {
                            f_close(&resultados_file);  // Se cierra fichero de resultados
                            f_close(&entradas_file);    // Se cierra fichero de entrada
                            f_mount(NULL, "", 1);       // Se desmonta sistema de archivos
                            return ERR_WRITE_FAIL_9; 
                        }   
                    }
                    //------------------------------------------------------------------------//

                    // Actualización de trazas capa 2
                    update_traces(capa2, trazasCapa2_3);

                    // Simulación capa 3
                    for (int i = 0; i < NUM_NEURONAS_CAPA_SALIDA; i++) {
                        simulate(outputCapa2, NUM_NEURONAS_CAPA_2, &capaSalida[i], &disparoCapa3, weightsCapa2_3[i]);
                    }

                    // STDP de capa 3
                    if (disparoCapa3) {
                        winnerIndex = winner_takes_all(capaSalida);
                        aplicar_STDP(winnerIndex, weightsCapa2_3, trazasCapa2_3);
                    }

                    for (int i = 0; i < NUM_NEURONAS_CAPA_SALIDA; i++) {
                        outputCapaSalida[i] = capaSalida[i].getPotencialSalida();
                    }


                    //-------------------- Registrar salidas de la red  --------------------//
                    offset = 0;
                    offset += sprintf(buff + offset, "SALIDA CAPA OUTPUT:\n");

                    for (int i = 0; i < NUM_NEURONAS_CAPA_SALIDA; i++) {
                        offset += sprintf(buff + offset, "%f ", outputCapaSalida[i]);
                    }

                    offset += sprintf(buff + offset, "\n\n");  // Añadir una nueva línea al final

                    // Escribir todo el contenido del buffer en el archivo de una sola vez
                    res = f_write(&resultados_file, buff, offset, &bw);
                    if (res != FR_OK || bw != strlen(buff)) {
                        f_close(&resultados_file);  // Se cierra fichero de resultados
                        f_close(&entradas_file);    // Se cierra fichero de entrada
                        f_mount(NULL, "", 1);       // Se desmonta sistema de archivos
                        return ERR_WRITE_FAIL_10;
                    }
                }
                //----------------------------------------------------------------------//

                event_count = -1;
                mod_timestamp = 0;

                f_close(&entradas_file);
            }
        }
    }

    // Se desmonta el sistema de archivos
    f_mount(NULL, "", 1);

    return NO_ERR;
}