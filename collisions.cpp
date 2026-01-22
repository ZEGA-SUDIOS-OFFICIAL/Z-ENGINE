#include "zega.h"

void PhysicsWorld::resolve_collisions() {
    for (size_t i = 0; i < nodes.size(); ++i) {
        // Star Collision
        float dx_s = star_x - nodes[i].x;
        float dy_s = star_y - nodes[i].y;
        float dist_s = std::sqrt(dx_s*dx_s + dy_s*dy_s);
        if (dist_s < star_r + ogc_margin) {
            float nx = dx_s / dist_s; float ny = dy_s / dist_s;
            nodes[i].x -= nx * (star_r + ogc_margin - dist_s);
            nodes[i].y -= ny * (star_r + ogc_margin - dist_s);
        }

        for (size_t k = i + 1; k < nodes.size(); ++k) {
            float dx = nodes[k].x - nodes[i].x;
            float dy = nodes[k].y - nodes[i].y;
            float dist = std::sqrt(dx*dx + dy*dy);
            float sum_r = nodes[i].radius + nodes[k].radius;
            if (dist < sum_r && dist > 0.001f) {
                float nx = dx / dist; float ny = dy / dist;
                float overlap = sum_r - dist;
                nodes[i].x -= nx * overlap * 0.5f;
                nodes[i].y -= ny * overlap * 0.5f;
                nodes[k].x += nx * overlap * 0.5f;
                nodes[k].y += ny * overlap * 0.5f;
            }
        }
    }
}