#include "particle_generator.h"
#include "particle_factory.h"

ParticleGen *ParticleFactory::getGenerator()
{
    ParticleGen *gen = new ParticleGen;
    return gen;
}

void ParticleFactory::destroyGenerator(ParticleGen *gen)
{
    delete gen;
}

ParticleGen *ParticleFactory::getBandGenerator(const std::pair<double, double> &rx, const std::pair<double, double> &ry, const std::pair<double, double> &rz)
{
    ParticleBand *gen = new ParticleBand();
    gen->setBandRange(rx, ry, rz);
    return gen;
}

ParticleGen *ParticleFactory::getBallGenerator(double x, double y, double z, double radius)
{
    ParticleBall *gen = new ParticleBall();
    gen->setBallRange(x, y, z, radius);
    return gen;
}

ParticleGen *ParticleFactory::getConeGenerator(double x, double y, double z, double angle, double heightTop, double heightBottom)
{
    ParticleCone *gen = new ParticleCone();
    gen->setConeRange(x, y, z, angle, heightTop, heightBottom);
    return gen;
}