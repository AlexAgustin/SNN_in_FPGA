#include <iostream>
#include <cmath>
#include <random>
#include <vector>

enum class EstadoNeurona {
    RECIBIENDO,
    ENESPERA
};

class NeuronaLIF {
private:
    double potencialMembrana;
    double potencialSalida;
    EstadoNeurona estado;
    const double potencialReposo = 0.0;
    const double thr = 4.0;
    const double decayFactor = exp(-1.0 / 10); //Cuanto mas grande el divisor, mas lento decae el potencial de membrana
    const double cooldown = 1.0;
    int conteoRefractario;

public:
    //Constructoras
    NeuronaLIF() : potencialMembrana(0.0), potencialSalida(0.0), estado(EstadoNeurona::RECIBIENDO), conteoRefractario(0) {}

    NeuronaLIF(double pMembrana, double pSalida, EstadoNeurona estado, double reposo, double threshold, double decay, double t_espera, int cont): 
        potencialMembrana(pMembrana), potencialSalida(pSalida), estado(estado), potencialReposo(reposo), thr(threshold), decayFactor(decay), cooldown(t_espera), conteoRefractario(cont) {}

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

void simulate(double* corrientesEntrada, NeuronaLIF* n);
