#include "../headers/neuron.h"



void integrar_entradas(double* corrientesEntrada, int numEntradas, NeuronaLIF* n, double* pesos) {
    std::cout << "Integrando entradas..." << std::endl;
    double aux= 0;

    BUCLE_AUX_INTEGRAR: for(int i=0; i<numEntradas; i++){
        aux += corrientesEntrada[i] * pesos[i];
        //std::cout << potencialMembrana << std::endl;
    }

    n->incPotencialMembrana(aux);

}

void simulate(double* corrientesEntrada, int numEntradas, NeuronaLIF* n, bool* disparo, double* pesos) {
        switch (n->getEstado()) {
            case EstadoNeurona::RECIBIENDO:
                // Estado en el que la neurona esta recibiendo entradas e integrandolas a su potencial
                // En caso de que al sumar las entradas de un instante el potencial de la neurona supere el threshold, se dispara un spike de salida
                
                //Calcula el valor del potencial aplicandole la fuga
                if(n->getPotencialMembrana() * n->getDecayFactor() >= n->getPotencialReposo()) 
                        n->setPotencialMembrana(n->getPotencialMembrana() * n->getDecayFactor()); 

                //Intregra todas las entradas en un instante de tiempo
                integrar_entradas(corrientesEntrada, numEntradas, n, pesos); 

                if (n->getPotencialMembrana() >= n->getThr()) {
                    *disparo=true;
                    n->setPotencialSalida(POTEN_SPIKE);  //Dispara el spike
                    std::cout << "Neurona disparada!" << std::endl;
                    n->setEstado(EstadoNeurona::ENESPERA);   //Cambio de estado
                }

                break;

            case EstadoNeurona::ENESPERA:
                // Estado en el que la neurona esta en tiempo de refraccion e ignora las entradas
                if (n->getPotencialSalida() == POTEN_SPIKE) {
                    n->setPotencialMembrana(n->getPotencialReposo());    //Devuelve al potencial de reposo el potencial de la membrana
                    n->setPotencialSalida(POTEN_NO_SPIKE);    //Devuelve a 0 la salida para que simule ese spike //Deberia mantenerse mas tiempo????????
                }

                if (n->getConteoRefractario() <= n->getCooldown()) { //Tiempo de espera
                    n->incConteoRefractario();
                } else {
                    n->setEstado(EstadoNeurona::RECIBIENDO);
                    n->rstConteoRefractario();
                }
                break;
        }
}

void simulate_entry(double corrienteEntrada, NeuronaLIF* n, bool* disparo) {
    // Estado en el que la neurona esta en tiempo de refraccion e ignora las entradas
    if (n->getPotencialSalida() == POTEN_SPIKE) {
        n->setPotencialMembrana(n->getPotencialReposo());    //Devuelve al potencial de reposo el potencial de la membrana
        n->setPotencialSalida(POTEN_NO_SPIKE);    //Devuelve a 0 la salida para que simule ese spike //Deberia mantenerse mas tiempo????????
    }
    
    //Intregra todas las entradas en un instante de tiempo
    n->incPotencialMembrana(corrienteEntrada);

    if (n->getPotencialMembrana() >= n->getThr()) {
        *disparo=true;
        n->setPotencialSalida(POTEN_SPIKE);  //Dispara el spike
        std::cout << "Neurona disparada!" << std::endl;
    }
}