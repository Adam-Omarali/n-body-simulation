#ifndef NBODY_DIAGNOSTICSCSVWRITER_HPP
#define NBODY_DIAGNOSTICSCSVWRITER_HPP

#include <fstream>

struct DiagnosticSample {
    double time;
    float energy_error;
    float momentum_error;
    float angular_momentum_error;
};

class DiagnosticsCsvWriter {
public:
    explicit DiagnosticsCsvWriter(const std::string& path)
        : file_(path)
    {
        if (!file_) {
            throw std::runtime_error("Could not open " + path);
        }

        file_ << std::setprecision(8);
        file_ << "time,energy_error,momentum_error,"
                 "angular_momentum_error\n";
    }

    void write(const DiagnosticSample& sample){
        file_ << sample.time << ','
              << sample.energy_error << ','
              << sample.momentum_error << ','
              << sample.angular_momentum_error << '\n';
    }

    void flush(){
        file_.flush();
    }

private:
    std::ofstream file_;
};

#endif