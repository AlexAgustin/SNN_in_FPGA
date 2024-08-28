#include <iostream>
#include <fstream>
#include <cmath>
#include <filesystem>
#include <string>
#include <cstdint>

#include "../headers/neuron.h"
#include "../headers/ff.h"

struct ProcessedEvent {
    double timestamp;
    double x;
    double y;
    double polarity;
};

int simulate_SNN ();