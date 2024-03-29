//--- Test Bench Related ---//
#define NUM_ITER 10
#define NUM_ENTRADAS 2
#define PROB_ENTRADA_SPIKE 0.5  //Valor decimal de la probabilidad de que la corriente de entrada sea un Spike

//--- Neuron Related ---//
#define THRESHOLD 6.0
#define POTEN_REPOSO 0.0
#define COOLDOWN 0.0
#define DECAY_EXP_FACTOR 100 //Valor que se usara para calcular el factor de decay  ==>   exp(-1, DECAY_EXP_FACTOR)

#define POTEN_SPIKE 2.0
#define POTEN_NO_SPIKE 0.0

//-- Network Related --//
#define NUM_NEURONAS_CAPA_1 2
#define NUM_NEURONAS_CAPA_2 3
#define NUM_NEURONAS_CAPA_3 1