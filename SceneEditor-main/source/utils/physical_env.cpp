#include "physical_env.h"
using namespace std;
Physical_env::Physical_env() : gravity_a(-9.8e-4) {}

void Physical_env::next_frame()
{
    for (auto iter1 = objects.begin(); iter1 != objects.end(); ++iter1) {
        double px1, py1, pz1;
        double w1, h1, d1;
        (*iter1)->getPos(px1, py1, pz1);
        (*iter1)->get_size(w1, h1, d1);
        auto iter2 = iter1;
        std::advance(iter2, 1);
        for (; iter2 != objects.end(); ++iter2) {
            double px2, py2, pz2;
            double w2, h2, d2;
            (*iter2)->getPos(px2, py2, pz2);
            (*iter2)->get_size(w2, h2, d2);

            double pt1x, pt1y, pt1z, pt2x, pt2y, pt2z;
            pt1x = std::max(px1 - w1 / 2, px2 - w2 / 2);
            pt1y = std::min(py1 + h1 / 2, py2 + h2 / 2);
            pt1z = std::max(pz1 - d1 / 2, pz2 - d2 / 2);
            pt2x = std::min(px1 + w1 / 2, px2 + w2 / 2);
            pt2y = std::max(py1 - h1 / 2, py2 - h2 / 2);
            pt2z = std::min(pz1 + d1 / 2, pz2 + d2 / 2);
            if (pt1x <= pt2x && pt1y >= pt2y && pt1z <= pt2z) {
                (*iter1)->apply_collision((*iter2));
            }
        }
    }
    for (auto iter = objects.begin(); iter != objects.end();) {
        (*iter)->apply_force(vector<double>{0.0f, 1.0f, 0.0f}, (*iter)->get_mass() * gravity_a);
        (*iter)->next_frame();
        if (detect) {
            (*iter)->set_destroy(detect(*iter));
        }
        if ((*iter)->should_be_destroy()) {
            delete *iter;
            iter = objects.erase(iter);
        } else {
            ++iter;
        }
    }
}

void Physical_env::draw(Shader &program) const
{
    for (auto iter = objects.begin(); iter != objects.end(); ++iter) {
        (*iter)->draw(program);
    }
}

void Physical_env::add_object(Physical_object_base *object)
{
    objects.push_back(object);
}

void Physical_env::delete_object(Physical_object_base *object)
{
    delete object;
    objects.remove(object);
}

Physical_env::~Physical_env()
{
    for (auto o : objects) {
        delete o;
    }
}

void Physical_env::set_gravity(double g)
{
    gravity_a = g;
}

double Physical_env::get_gravity()
{
    return gravity_a;
}

void Physical_env::set_detect(const std::function<bool (Physical_object_base *)> func)
{
    detect = func;
}