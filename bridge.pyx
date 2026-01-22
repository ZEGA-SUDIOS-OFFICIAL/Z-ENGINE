# distutils: language = c++
from libcpp.vector cimport vector
from libcpp.pair cimport pair

cdef extern from "zega.h":
    struct Node:
        float x, y, radius
    struct Constraint:
        int n1, n2
        float stiffness
    cdef cppclass ParticleEmitter:
        void spawn(float x, float y, float vx, float vy)
        vector[pair[vector[pair[float, float]], float]] get_traced_paths()
    cdef cppclass PhysicsWorld:
        PhysicsWorld(float sx, float sy) except +
        int add_node(float x, float y, float vx, float vy, float r)
        void add_constraint(int i1, int i2, float stiff)
        void update()
        vector[Node] nodes
        vector[Constraint] constraints # Expose this!
        ParticleEmitter* emitter

cdef class PyPhysicsWorld:
    cdef PhysicsWorld* cpp_world
    def __cinit__(self, float sx, float sy): self.cpp_world = new PhysicsWorld(sx, sy)
    def __dealloc__(self): del self.cpp_world
    
    def add_node(self, x, y, vx, vy, r): return self.cpp_world.add_node(x, y, vx, vy, r)
    def add_constraint(self, i1, i2, s): self.cpp_world.add_constraint(i1, i2, s)
    def update(self): self.cpp_world.update()
    
    def spawn_particle(self, x, y, vx, vy): self.cpp_world.emitter.spawn(x, y, vx, vy)
    def get_particles(self): return self.cpp_world.emitter.get_traced_paths()
    
    def get_nodes_py(self):
        return [{"x": n.x, "y": n.y, "r": n.radius} for n in self.cpp_world.nodes]

    # New function to get beams for rendering
    def get_beams_py(self):
        return [(c.n1, c.n2) for c in self.cpp_world.constraints]