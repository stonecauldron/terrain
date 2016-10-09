#pragma once
#include "icg_common.h"

class Trackball {
public:
    Trackball() : _radius(1.0f) {}

    void begin_drag(float x, float y) {
      _anchor_pos = vec3(x, y, 0.0f);
      project_onto_surface(_anchor_pos);
    }

    mat4 drag(float x, float y) {
      vec3 current_pos = vec3(x, y, 0.0f);
      project_onto_surface(current_pos);

      mat4 rotation = mat4::Identity();
      vec3 rotAxis = _anchor_pos.cross(current_pos).normalized();
      float angle = acos(_anchor_pos.normalized().dot(current_pos.normalized()));
      rotation = Eigen::Affine3f(Eigen::AngleAxisf(angle, rotAxis)).matrix();
      return rotation;
    }

private:
    void project_onto_surface(vec3& p) const {
        if (2*p.x()*p.x() + 2*p.y()*p.y() <= _radius*_radius) {
            // p is inside the sphere
            p.z() = sqrt(_radius*_radius - (p.x()*p.x() + p.y()*p.y()));
        }
        else {
            // p is inside the hypersheet 
            p.z() = ((_radius*_radius)/2.0f)/(sqrt(p.x()*p.x() + p.y()*p.y()));
        }
    }

    float _radius;
    vec3 _anchor_pos;
    mat4 _rotation;
};
