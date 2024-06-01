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
#define NUM_NEURONAS_CAPA_ENTRADA 2
//#define NUM_NEURONAS_CAPA_2 3
#define NUM_NEURONAS_CAPA_SALIDA 1
#define W_INIT 0.5
#define LEARNING_RATE 0.01
#define A 0.5
#define ALPHA 0.1
#define TAU_PLUS 20.0
#define DT 0.1
#define TRAZA_INIT 0.0
//#define RESET_TRAZA 1.0

extern bool disparoCapa1;
extern bool disparoCapa2;
//bool disparoCapa3=false;