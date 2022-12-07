#ifndef PARTICLE_GENERATOR
#define PARTICLE_GENERATOR

#include <utility>

class Physical_object_base;

class ParticleGen {
public:
    ParticleGen();
    void setParticleRateRange(const std::pair<double, double> &vx, const std::pair<double, double> &vy, const std::pair<double, double> &vz);
    void setParticlePosRange(const std::pair<double, double> &px, const std::pair<double, double> &py, const std::pair<double, double> &pz);
    void setParticleSize(double w, double h, double d);
    virtual Physical_object_base *genParticle();
    virtual ~ParticleGen() = default;
protected:
    std::pair<double, double> mVx, mVy, mVz;
    std::pair<double, double> mPosX, mPosY, mPosZ;
    double sizeW, sizeH, sizeD;
};

class ParticleBand : public ParticleGen {
public:
    void setBandRange(const std::pair<double, double> &rx, const std::pair<double, double> &ry, const std::pair<double, double> &rz);
    Physical_object_base *genParticle() override;
private:
    std::pair<double, double> mBandX, mBandY, mBandZ;
};

class ParticleBall : public ParticleGen {
public:
    void setBallRange(double x, double y, double z, double radius);
    Physical_object_base *genParticle() override;
private:
    double px, py, pz, radius;
};

class ParticleCone : public ParticleGen {
public:
    void setConeRange(double x, double y, double z, double angle, double heightTop, double heightBottom);
    Physical_object_base *genParticle() override;
private:
    double px, py, pz, angle, heightTop, heightBottom;
};
#endif