#ifndef PHYSICAL_ENV
#define PHYSICAL_ENV

#include <list>
#include <functional>
#include "physical_object.h"

class Physical_env {
public:
    Physical_env();
    ~Physical_env();
    void next_frame();
    void draw(Shader &program) const;
    void add_object(Physical_object_base *object);
    void delete_object(Physical_object_base *object);
    void set_gravity(double g);
    double get_gravity();
    void set_detect(const std::function<bool (Physical_object_base *)> func);
private:
    double gravity_a;
    std::list<Physical_object_base *> objects;
    std::function<bool (Physical_object_base *)> detect;
};

#endif