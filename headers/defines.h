//--- Test Bench Related ---//
#define NUM_ITER 40
#define NUM_ENTRADAS 2
#define PROB_ENTRADA_SPIKE 0.5  //Valor decimal de la probabilidad de que la corriente de entrada sea un Spike

//--- Neuron Related ---//
#define THRESHOLD 4.0
#define POTEN_REPOSO 0.0
#define COOLDOWN 0.0
#define DECAY_EXP_FACTOR 100 //Valor que se usara para calcular el factor de decay  ==>   exp(-1, DECAY_EXP_FACTOR)ç

#define POTEN_SPIKE 2.0
#define POTEN_NO_SPIKE 0.0

//-- Network Related --//
#define GRID 16
#define NUM_NEURONAS_CAPA_ENTRADA GRID*GRID*2
#define NUM_NEURONAS_CAPA_2 70
#define NUM_NEURONAS_CAPA_SALIDA 10
#define W_INIT 0.5
#define LEARNING_RATE 0.01
#define A 0.5
#define ALPHA 0.1
#define TAU_PLUS 20.0
#define DT 0.1
#define TRAZA_INIT 0.0
//#define RESET_TRAZA 1.0
#define NUM_DELAYS 5
#define MAX_EVENTS 100

    //Codigos de errores de ficheros de la red
#define NO_ERR 0
#define ERR_MOUNT_FAIL 100
#define ERR_OPEN_RESULT_FILE 200

#define ERR_WRITE_FAIL_1 301
#define ERR_WRITE_FAIL_2 302
#define ERR_WRITE_FAIL_3 303
#define ERR_WRITE_FAIL_4 304
#define ERR_WRITE_FAIL_5 305
#define ERR_WRITE_FAIL_6 306
#define ERR_WRITE_FAIL_7 307
#define ERR_WRITE_FAIL_8 308
#define ERR_WRITE_FAIL_9 309
#define ERR_WRITE_FAIL_10 310
#define ERR_WRITE_FAIL_11 311

extern bool disparoCapa1;
extern bool disparoCapa2;
extern bool disparoCapa3;