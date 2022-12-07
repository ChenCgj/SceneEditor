#include <cmath>
#include "physical_object.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

using namespace std;

Physical_object_base::Physical_object_base(double posX, double posY, double posZ, double vx, double vy, double vz, double ax, double ay, double az, double mass)
    : mVx(vx), mVy(vy), mVz(vz), mAx(ax), mAy(ay), mAz(az), mPosX(posX), mPosY(posY), mPosZ(posZ), mMass(mass), destroy(false),
    sizeX(0), sizeY(sizeX), sizeZ(sizeX), ignoreForce(false) {}

void Physical_object_base::apply_force(std::vector<double> dir, double force)
{
    double a = force / mMass;
    double len = sqrt(pow(dir[0], 2) + pow(dir[1], 2) + pow(dir[2], 2));
    mAx = a * dir[0] / len;
    mAy = a * dir[1] / len;
    mAz = a * dir[2] / len;
}

void Physical_object_base::next_frame()
{
    if (!ignoreForce) {
        mVx += mAx;
        mVy += mAy;
        mVz += mAz;
    }

    mPosX += mVx;
    mPosY += mVy;
    mPosZ += mVz;

    if (detect && detect(this)) {
        set_destroy(true);
    }
}

void Physical_object_base::set_destroy_detect(const std::function<bool (Physical_object_base *)> func)
{
    detect = func;
}

void Physical_object_base::apply_collision(Physical_object_base *another)
{
    if (ignoreForce && another->ignoreForce) {
        return;
    } else if (!ignoreForce && !another->ignoreForce) {
        std::swap(mVx, another->mVx);
        std::swap(mVy, another->mVy);
        std::swap(mVz, another->mVz);
    } else if (ignoreForce) {
        another->mVx = -another->mVx;
        another->mVy = -another->mVy;
        another->mVz = -another->mVz;
    } else {
        mVx = -mVx;
        mVy = -mVy;
        mVz = -mVz;
    }
}

double Physical_object_base::get_mass() const
{
    return mMass;
}

void Physical_object::load_model(const std::string &filename, const glm::mat4 &matrix)
{
    model.loadModel(filename);
    model.setBaseMatrix(matrix);
}

Physical_object::Physical_object(double posX, double posY, double posZ, double vx, double vy, double vz, double ax, double ay, double az, double mass) :
    Physical_object_base(posX, posY, posZ, vx, vy, vz, ax, ay, az, mass)
{
    model.loadModel();
    model.getModelSize(sizeX, sizeY, sizeZ);
}

void Physical_object::draw(Shader &program)
{
    model.setTranslateMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(mPosX, mPosY, mPosZ)));
    model.draw(program);
}

void Physical_object::next_frame()
{
    Physical_object_base::next_frame();
}

bool Physical_object_base::should_be_destroy()
{
    return destroy;
}

void Physical_object_base::getPos(double &px, double &py, double &pz)
{
    px = mPosX, py = mPosY, pz = mPosZ;
}

void Physical_object_base::getVelocity(double &vx, double &vy, double &vz)
{
    vx = mVx, vy = mVy, vz = mVz;
}

void Physical_object_base::set_baseMatrix(const glm::mat4 &matrix) {}

void Physical_object_base::get_size(double &w, double &h, double &d)
{
    w = sizeX;
    h = sizeY;
    d = sizeZ;
}

void Physical_object_base::setIgnoreForce(bool flag)
{
    ignoreForce = flag;
}

bool Physical_object_base::getIgnoreForce() const
{
    return ignoreForce;
}

Physical_object::~Physical_object()
{
    model.unload();
}

void Physical_object::set_baseMatrix(const glm::mat4 &matrix)
{
    model.setBaseMatrix(matrix);
}

void Physical_object_base::set_destroy(bool desFlag)
{
    destroy = desFlag;
}

void Physical_object::get_size(double &w, double &h, double &d)
{
    model.getModelSize(w, h, d);
}

void Physical_object::scale(double scaleX, double scaleY, double scaleZ)
{
    model.setBaseMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(scaleX, scaleY, scaleZ)) * model.getBaseMatrix());
    sizeX *= scaleX;
    sizeY *= scaleY;
    sizeZ *= scaleZ;
}

void Physical_object::rotate(const glm::vec3 axis, double angle)
{
    model.setBaseMatrix(glm::rotate(glm::mat4(1.0f), static_cast<float>(angle), axis) * model.getBaseMatrix());
}

void Physical_object::move(double dx, double dy, double dz)
{
    model.setBaseMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(dx, dy, dz)) * model.getBaseMatrix());
}