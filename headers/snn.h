#include <iostream>
#include <fstream>
#include <cmath>
#include <filesystem>
#include <string>
#include <cstdint>

namespace fs = std::filesystem;


#include "../headers/neuron.h"
#include "../headers/ff.h"

struct ProcessedEvent {
    double timestamp;
    double x;
    double y;
    double polarity;
};

void simulate_SNN ();