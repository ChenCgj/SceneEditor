#ifndef PARTICLE_FACTORY
#define PARTICLE_FACTORY

#include <utility>
class ParticleGen;

class ParticleFactory {
public:
    enum GenType {
        Band
    };
    ParticleGen *getGenerator();
    ParticleGen *getBandGenerator(const std::pair<double, double> &rx, const std::pair<double, double> &ry, const std::pair<double, double> &rz);
    ParticleGen *getBallGenerator(double x, double y, double z, double radius);
    ParticleGen *getConeGenerator(double x, double y, double z, double angle, double heightTop, double heightBottom);
    void destroyGenerator(ParticleGen *gen);
};

#endif