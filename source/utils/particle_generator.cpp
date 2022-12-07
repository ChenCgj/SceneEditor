#include "gtc/matrix_transform.hpp"
#include "physical_object.h"
#include "particle_generator.h"
#include <ctime>

double getRand(const std::pair<double, double> &range)
{
    static bool init = true;
    if (init) {
        srand(time(nullptr));
        init = false;
    }
    if (range.first >= range.second) {
        return range.first;
    }
    return rand() * 1.0 / RAND_MAX * (range.second - range.first) + range.first;
}

ParticleGen::ParticleGen() : mVx(std::make_pair(-0.01, 0.01)), mVy(mVx), mVz(mVx),
    mPosX(std::make_pair(-1, 1)), mPosY(mPosX), mPosZ(mPosX),
    sizeW(0.01), sizeH(0.01), sizeD(0.01) {}

void ParticleGen::setParticlePosRange(const std::pair<double, double> &px, const std::pair<double, double> &py, const std::pair<double, double> &pz)
{
    mPosX = px;
    mPosY = py;
    mPosZ = pz;
}

void ParticleGen::setParticleRateRange(const std::pair<double, double> &vx, const std::pair<double, double> &vy, const std::pair<double, double> &vz)
{
    mVx = vx;
    mVy = vy;
    mVz = vz;
}

void ParticleGen::setParticleSize(double w, double h, double d)
{
    sizeW = w;
    sizeH = h;
    sizeD = d;
}

Physical_object_base *ParticleGen::genParticle()
{
    double px = getRand(mPosX);
    double py = getRand(mPosY);
    double pz = getRand(mPosZ);
    double vx = getRand(mVx);
    double vy = getRand(mVy);
    double vz = getRand(mVz);
    Physical_object *o = new Physical_object(px, py, pz, vx, vy, vz);
    double w, h, d;
    o->get_size(w, h, d);
    double minScale = std::min(sizeW / w, std::min(sizeH / h, sizeD / d));
    o->scale(minScale, minScale, minScale);
    return o;
}

void ParticleBand::setBandRange(const std::pair<double, double> &rx, const std::pair<double, double> &ry, const std::pair<double, double> &rz)
{
    mBandX = rx;
    mBandY = ry;
    mBandZ =rz;
}

Physical_object_base *ParticleBand::genParticle()
{
    Physical_object_base *o = ParticleGen::genParticle();
    auto detect = [=](Physical_object_base *object) -> bool {
        double x, y, z;
        object->getPos(x, y, z);
        if (mBandX.first <= mBandX.second && (x < mBandX.first || x > mBandX.second)) {
            return true;
        }
        if (mBandY.first <= mBandY.second && (y < mBandY.first || y > mBandY.second)) {
            return true;
        }
        if (mBandZ.first <= mBandZ.second && (z < mBandZ.first || z > mBandZ.second)) {
            return true;
        }
        return false;
    };
    o->set_destroy_detect(detect);
    return o;
}

void ParticleBall::setBallRange(double x, double y, double z, double radius)
{
    px = x;
    py = y;
    pz = z;
    this->radius = radius;
}

Physical_object_base *ParticleBall::genParticle()
{
    Physical_object_base *o = ParticleGen::genParticle();
    auto detect = [=](Physical_object_base *object) -> bool {
        double x, y, z;
        o->getPos(x, y, z);
        if (pow(x - px, 2) + pow(y - py, 2) + pow(z - pz, 2) > radius * radius) {
            return true;
        }
        return false;
    };
    o->set_destroy_detect(detect);
    return o;
}

void ParticleCone::setConeRange(double x, double y, double z, double angle, double heightTop, double heightBottom)
{
    px = x;
    py = y;
    pz = z;
    this->angle = angle;
    this->heightTop = heightTop;
    this->heightBottom = heightBottom;
}

Physical_object_base *ParticleCone::genParticle()
{
    Physical_object_base *o = ParticleGen::genParticle();
    auto detect = [=](Physical_object_base *object) -> bool {
        double x, y, z;
        o->getPos(x, y, z);
        if ((heightBottom >= 0 && y < pz - heightBottom) || (heightTop >= 0 && y > pz + heightTop)) {
            return true;
        }
        double len = sqrt(pow(x - px, 2) + pow(y - py, 2) + pow(z - pz, 2));
        if (len && fabs(y - py) / len < cos(angle / 2)) {
            return true;
        }
        return false;
    };
    o->set_destroy_detect(detect);
    return o;
}