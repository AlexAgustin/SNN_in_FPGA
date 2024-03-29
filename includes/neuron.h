#include <iostream>
#include <cmath>
#include <random>
#include <vector>

#include "../includes/defines.h"

enum class EstadoNeurona {
    RECIBIENDO,
    ENESPERA
};

class NeuronaLIF {
private:
    double potencialMembrana;
    double potencialSalida;
    EstadoNeurona estado;
    const double potencialReposo = POTEN_REPOSO;
    const double thr = THRESHOLD;
    const double decayFactor = exp(-1.0 / DECAY_EXP_FACTOR); //Cuanto mas grande el divisor, mas lento decae el potencial de membrana
    const double cooldown = COOLDOWN;
    int conteoRefractario;

public:
    //Constructoras
    NeuronaLIF() : potencialMembrana(POTEN_REPOSO), potencialSalida(POTEN_REPOSO), estado(EstadoNeurona::RECIBIENDO), conteoRefractario(0) {}
    
    //Getters y setters
    double getPotencialMembrana() const {
        return potencialMembrana;
    }

    void incPotencialMembrana(double increase) {
        potencialMembrana += increase;
    }

    double getPotencialSalida() const {
        return potencialSalida;
    }

    EstadoNeurona getEstado() const {
        return estado;
    }

    double getPotencialReposo() const {
        return potencialReposo;
    }

    double getThr() const {
        return thr;
    }

    double getDecayFactor() const {
        return decayFactor;
    }
    
    double getCooldown() const {
        return cooldown;
    }

    double getConteoRefractario() const {
        return conteoRefractario;
    }

    void setPotencialMembrana(double poten) {
        potencialMembrana = poten;
    }

    void setEstado(EstadoNeurona state) {
        estado = state;
    }

    void setPotencialSalida(double poten) {
        potencialSalida = poten;
    }

    void incConteoRefractario() {
        conteoRefractario++;
    }

    void rstConteoRefractario() {
        conteoRefractario = 0;
    }
};

void simulate(double* corrientesEntrada, int numEntradas, NeuronaLIF* n);
