#ifndef PHYSICAL_OBJECT
#define PHYSICAL_OBJECT

#include <vector>
#include <functional>

#include "model.h"

class Shader;
class Physical_object_base {
public:
    Physical_object_base(double posX = 0, double posY = 0, double posZ = 0, double vx = 0, double vy = 0, double vz = 0, double ax = 0, double ay = 0, double az = 0, double mass = 1.0f);
    virtual void next_frame();
    virtual void draw(Shader &program) = 0;
    virtual void apply_force(std::vector<double> dir, double strength);
    virtual void apply_collision(Physical_object_base *another);
    virtual ~Physical_object_base() = default;
    bool should_be_destroy();
    void set_destroy(bool desFlag);
    void get_size(double &w, double &h, double &d);
    double get_mass() const;
    void getPos(double &px, double &py, double &pz);
    void getVelocity(double &vx, double &vy, double &vz);
    void set_destroy_detect(const std::function<bool (Physical_object_base *)> func);
    void setIgnoreForce(bool flag);
    bool getIgnoreForce() const;
protected:
    virtual void set_baseMatrix(const glm::mat4 &matrix);
    double mVx, mVy, mVz;
    double mAx, mAy, mAz;
    double mPosX, mPosY, mPosZ;
    double mMass;
    bool destroy;
    // std::pair<double, double> sizeX, sizeY, sizeZ;
    double sizeX, sizeY, sizeZ;
    bool ignoreForce;
    // void set_mass(double mass);
    // double get_mass();
private:
    std::function<bool (Physical_object_base *)> detect;
};

class Physical_object : public Physical_object_base {
public:
    Physical_object(double posX = 0, double posY = 0, double posZ = 0,
        double vx = 0, double vy = 0, double vz = 0,
        double ax = 0, double ay = 0, double az = 0,
        double mass = 1.0f);
    void load_model(const std::string &filename, const glm::mat4 &matrix);
    void draw(Shader &program) override;
    void next_frame() override;
    void scale(double scaleX, double scaleY, double scaleZ);
    void rotate(const glm::vec3 axis, double angle);
    void move(double dx, double dy, double dz);
    void get_size(double &w, double &h, double &d);
    ~Physical_object();
protected:
    void set_baseMatrix(const glm::mat4 &matrix) override;
private:
    Model model;
};

#endif