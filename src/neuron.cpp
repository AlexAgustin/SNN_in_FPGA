#include "../includes/neuron.h"
#include "../includes/defines.h"

void integrarEntradas(double* corrientesEntrada, NeuronaLIF* n) {
    std::cout << "Integrando entradas..." << std::endl;
    double aux= 0;

    BUCLE_AUX_INTEGRAR: for(int i=0; i<NUM_ENTRADAS; i++){
        aux += corrientesEntrada[i];
        //std::cout << potencialMembrana << std::endl;
    }

    n->incPotencialMembrana(aux);

}

void simulate(double* corrientesEntrada, NeuronaLIF* n) {
        switch (n->getEstado()) {
            case EstadoNeurona::RECIBIENDO:
                // Estado en el que la neurona esta recibiendo entradas e integrandolas a su potencial
                // En caso de que al sumar las entradas de un instante el potencial de la neurona supere el threshold, se dispara un spike de salida
                // Si no se supero el thr, se le restara al potencial la fuga

                integrarEntradas(corrientesEntrada, n);; //Intregra todas las entradas en un instante de tiempo

                if (n->getPotencialMembrana() >= n->getThr()) {
                    n->setPotencialMembrana(n->getPotencialReposo());    //Devuelve al potencial de reposo el potencial de la membrana
                    n->setPotencialSalida(POTEN_SPIKE);  //Dispara el spike
                    std::cout << "Neurona disparada!" << std::endl;
                    n->setEstado(EstadoNeurona::ENESPERA);   //Cambio de estado
                }
                else 
                    //Calcula el valor del potencial aplicandole la fuga
                    if(n->getPotencialMembrana() * n->getDecayFactor() >= n->getPotencialReposo()) 
                        n->setPotencialMembrana(n->getPotencialMembrana() * n->getDecayFactor()); 
                break;

            case EstadoNeurona::ENESPERA:
                // Estado en el que la neurona esta en tiempo de refraccion e ignora las entradas
                if (n->getPotencialSalida() == POTEN_SPIKE) n->setPotencialSalida(POTEN_NO_SPIKE);    //Devuelve a 0 la salida para que simule ese spike //Deberia mantenerse mas tiempo????????
                
                if (n->getConteoRefractario() < n->getCooldown()) { //Tiempo de espera
                    n->incConteoRefractario();
                } else {
                    n->setEstado(EstadoNeurona::RECIBIENDO);
                    n->rstConteoRefractario();
                }
                break;
        }
    std::cout << "Potencial de la membrana de la neurona: " << n->getPotencialMembrana() << std::endl;
    std::cout << "Potencial de salida de la neurona: " << n->getPotencialSalida() << std::endl;
}