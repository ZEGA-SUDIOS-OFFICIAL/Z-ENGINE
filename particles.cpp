#include "zega.h"

ParticleEmitter::ParticleEmitter(float g, float fr, float ogc)
    : max_history(10), G(g), friction(fr), ogc_margin(ogc) {}

void ParticleEmitter::spawn(float x, float y, float vx, float vy) {
    Particle p;
    p.x = x; p.y = y;
    p.old_x = x - vx; p.old_y = y - vy;
    p.life = 1.0f;
    p.history.clear();
    particles.push_back(p);
}

void ParticleEmitter::update(float px, float py, float pr, float sx, float sy, float sr, float m_p, float m_s) {
    for (int i = static_cast<int>(particles.size()) - 1; i >= 0; --i) {
        float acc_x = 0.0f;
        float acc_y = 0.0f;

        // Star gravity
        float dx = sx - particles[i].x;
        float dy = sy - particles[i].y;
        float d_sq = dx * dx + dy * dy;
        float dist = std::sqrt(d_sq);
        if (dist > 1.0f) {
            float f = (G * m_s) / d_sq;
            acc_x += (dx / dist) * f;
            acc_y += (dy / dist) * f;
        }

        // Planet gravity
        dx = px - particles[i].x;
        dy = py - particles[i].y;
        d_sq = dx * dx + dy * dy;
        dist = std::sqrt(d_sq);
        if (dist > 1.0f) {
            float f = (G * m_p) / d_sq;
            acc_x += (dx / dist) * f;
            acc_y += (dy / dist) * f;
        }

        // Integration
        float vx = (particles[i].x - particles[i].old_x) * friction;
        float vy = (particles[i].y - particles[i].old_y) * friction;
        particles[i].old_x = particles[i].x;
        particles[i].old_y = particles[i].y;
        particles[i].x += vx + acc_x;
        particles[i].y += vy + acc_y;

        // Life decrease
        particles[i].life -= 0.04f;

        // Check star destruction and too far
        dx = sx - particles[i].x;
        dy = sy - particles[i].y;
        dist = std::sqrt(dx * dx + dy * dy);
        if (dist < sr || dist > 2000.0f) {
            particles.erase(particles.begin() + i);
            continue;
        }

        // Planet collision
        dx = px - particles[i].x;
        dy = py - particles[i].y;
        dist = std::sqrt(dx * dx + dy * dy);
        if (dist < pr + ogc_margin) {
            float nx = -dx / dist;  // outward normal from center to particle
            float ny = -dy / dist;
            float overlap = pr + ogc_margin - dist;
            particles[i].x += nx * overlap;
            particles[i].y += ny * overlap;

            // Dynamic bounce
            vx = particles[i].x - particles[i].old_x;
            vy = particles[i].y - particles[i].old_y;
            float dot = vx * nx + vy * ny;
            if (dot < 0.0f) {  // approaching
                float e = 0.6f;
                float j = -(1.0f + e) * dot;
                particles[i].old_x -= j * nx;
                particles[i].old_y -= j * ny;
            }
        }

        // Add to history if moved enough to prevent clumping
        float cp_x = particles[i].x;
        float cp_y = particles[i].y;
        bool add_to_history = true;
        if (!particles[i].history.empty()) {
            float hist_dist = std::hypot(cp_x - particles[i].history.front().x, cp_y - particles[i].history.front().y);
            if (hist_dist <= 1.0f) {
                add_to_history = false;
            }
        }
        if (add_to_history) {
            Point cp{cp_x, cp_y};
            particles[i].history.push_front(cp);
        }
        if (particles[i].history.size() > static_cast<size_t>(max_history)) {
            particles[i].history.pop_back();
        }

        if (particles[i].life <= 0.0f) {
            particles.erase(particles.begin() + i);
        }
    }
}

std::vector<std::pair<std::vector<std::pair<float, float>>, float>> ParticleEmitter::get_traced_paths() {
    std::vector<std::pair<std::vector<std::pair<float, float>>, float>> res;
    for (auto& p : particles) {
        if (p.history.size() < 2) continue;
        std::vector<std::pair<float, float>> path;
        for (auto& pt : p.history) {
            path.push_back({pt.x, pt.y});
        }
        res.push_back({path, p.life});
    }
    return res;
}