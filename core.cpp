#include "zega.h"

PhysicsWorld::PhysicsWorld(float sx, float sy)
    : star_x(sx), star_y(sy), star_r(70.0f), mass_star(60000.0f), gravity_star(1.0f),
      planet_x(0.0f), planet_y(0.0f), planet_r(35.0f), mass_planet(10000.0f), gravity_planet(1.0f),
      G(0.2f), friction(0.998f), ogc_margin(2.0f) 
{
    emitter = new ParticleEmitter(G, friction, ogc_margin);
}

PhysicsWorld::~PhysicsWorld() {
    if (emitter) delete emitter;
}

int PhysicsWorld::add_node(float x, float y, float vx, float vy, float r) {
    Node n = {x, y, x - vx, y - vy, 0, 0, r, 1.0f, 0};
    nodes.push_back(n);
    return static_cast<int>(nodes.size() - 1);
}

void PhysicsWorld::add_constraint(int i1, int i2, float stiff) {
    float dx = nodes[i1].x - nodes[i2].x;
    float dy = nodes[i1].y - nodes[i2].y;
    constraints.push_back({i1, i2, std::sqrt(dx*dx + dy*dy), stiff});
}

void PhysicsWorld::update() {
    emitter->update(planet_x, planet_y, planet_r, star_x, star_y, star_r, mass_planet, mass_star);

    for (auto& n : nodes) {
        if (n.pinned) continue;
        // Star Gravity
        float dx = star_x - n.x; float dy = star_y - n.y;
        float d = std::sqrt(dx*dx + dy*dy);
        if (d > 1.0f) {
            float f = (G * mass_star * gravity_star) / (d*d);
            n.acc_x += (dx/d)*f; n.acc_y += (dy/d)*f;
        }
        // Verlet
        float vx = (n.x - n.old_x) * friction;
        float vy = (n.y - n.old_y) * friction;
        n.old_x = n.x; n.old_y = n.y;
        n.x += vx + n.acc_x; n.y += vy + n.acc_y;
        n.acc_x = 0; n.acc_y = 0;
    }

    for (int j = 0; j < 6; j++) {
        for (auto& c : constraints) {
            float dx = nodes[c.n2].x - nodes[c.n1].x;
            float dy = nodes[c.n2].y - nodes[c.n1].y;
            float dist = std::sqrt(dx*dx + dy*dy);
            if (dist > 0.001f) {
                float diff = (c.rest_len - dist) / dist * 0.5f * c.stiffness;
                nodes[c.n1].x -= dx * diff; nodes[c.n1].y -= dy * diff;
                nodes[c.n2].x += dx * diff; nodes[c.n2].y += dy * diff;
            }
        }
    }
    resolve_collisions();
}