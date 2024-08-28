#include "../headers/ff.h"
#include "../headers/escritura_sd.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <filesystem>
#include <string>
#include <cstdint>

void test(){
    //------------------------- Montaje de la Tarjeta SD -------------------------//
    FATFS fs;
    FRESULT res;

    res = f_mount(&fs, "", 1);
    if (res != FR_OK) {
        std::cout << "Error mounting SD card: " << res << "\n" << std::endl;
        return;
    }

    //------------------------- Inicializacion de la escritura en SD -------------------------//
    FIL resultados_file;
    UINT bw;
    char buff[1024];
    int offset = 0;

    res = f_open(&resultados_file, "resultados.txt", FA_WRITE | FA_CREATE_ALWAYS);
    if (res != FR_OK) {
        std::cout << "Error opening resultados file: " << res << "\n" << std::endl;
        return;
    }

    sprintf(buff, "Hello World!!");
    res = f_write(&resultados_file,buff,strlen(buff),&bw);
    if (res != FR_OK || bw != strlen(buff)) {
        f_close(&resultados_file);  // Se cierra fichero de resultados
        f_mount(NULL, "", 1);       // Se desmonta sistema de archivos
        return;
    }

    f_close(&resultados_file);  // Se cierra fichero de resultados
    f_mount(NULL, "", 1);       // Se desmonta sistema de archivos
    return;
}