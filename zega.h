#ifndef ZEGA_H
#define ZEGA_H

#include <vector>
#include <deque>
#include <utility>
#include <cmath>

struct Point { float x, y; };

struct Particle {
    float x, y, old_x, old_y, life;
    std::deque<Point> history;
};

class ParticleEmitter {
public:
    std::vector<Particle> particles;
    int max_history;
    float G, friction, ogc_margin;

    ParticleEmitter(float g, float fr, float ogc);
    // This is the symbol the linker is looking for:
    void spawn(float x, float y, float vx, float vy);
    void update(float px, float py, float pr, float sx, float sy, float sr, float m_p, float m_s);
    std::vector<std::pair<std::vector<std::pair<float, float>>, float>> get_traced_paths();
};

struct Node {
    float x, y, old_x, old_y, acc_x, acc_y, radius, mass;
    int pinned;
};

struct Constraint {
    int n1, n2;
    float rest_len, stiffness;
};

class PhysicsWorld {
public:
    std::vector<Node> nodes;
    std::vector<Constraint> constraints;
    ParticleEmitter* emitter;

    float star_x, star_y, star_r, mass_star, gravity_star;
    float planet_x, planet_y, planet_r, mass_planet, gravity_planet;
    float G, friction, ogc_margin;

    PhysicsWorld(float sx, float sy);
    ~PhysicsWorld(); 

    int add_node(float x, float y, float vx, float vy, float r);
    void add_constraint(int i1, int i2, float stiff);
    void update();
    void resolve_collisions();
};

#endif