import pygame, math, random
from zega_engine import PyPhysicsWorld

WIDTH, HEIGHT = 1280, 720
ZEGA_GREEN = (88, 240, 27) # #58f01b
STAR_POS = (WIDTH//2, HEIGHT//2)

def spawn_zega_mesh(world, x, y, vx, vy):
    rows, cols, spacing = 5, 5, 25
    grid = []
    for r in range(rows):
        row = [world.add_node(x+c*spacing, y+r*spacing, vx, vy, 4) for c in range(cols)]
        grid.append(row)
    
    # Add Beams (Constraints)
    for r in range(rows):
        for c in range(cols):
            if c < cols-1: world.add_constraint(grid[r][c], grid[r][c+1], 0.9) # Horiz
            if r < rows-1: world.add_constraint(grid[r][c], grid[r+1][c], 0.9) # Vert
            if r < rows-1 and c < cols-1: # Diagonals for structural "Nitrogen" strength
                world.add_constraint(grid[r][c], grid[r+1][c+1], 0.8)
                world.add_constraint(grid[r+1][c], grid[r][c+1], 0.8)

def main():
    pygame.init()
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    clock = pygame.time.Clock()
    world = PyPhysicsWorld(float(STAR_POS[0]), float(STAR_POS[1]))
    
    spawn_zega_mesh(world, 100, 100, 12, 10)

    while True:
        screen.fill((5, 5, 12))
        for event in pygame.event.get():
            if event.type == pygame.QUIT: return
            if event.type == pygame.MOUSEBUTTONDOWN:
                mx, my = pygame.mouse.get_pos()
                spawn_zega_mesh(world, mx, my, 8, 8)

        world.update()
        
        # 1. DRAW THE STAR
        pygame.draw.circle(screen, (255, 200, 50), STAR_POS, 70)

        # 2. DRAW THE BEAMS (This was the missing part!)
        nodes = world.get_nodes_py()
        beams = world.get_beams_py() # Fetching directly from C++ now
        for b1, b2 in beams:
            p1 = (int(nodes[b1]['x']), int(nodes[b1]['y']))
            p2 = (int(nodes[b2]['x']), int(nodes[b2]['y']))
            # Drawing the beam in a slightly darker green for depth
            pygame.draw.line(screen, (40, 150, 20), p1, p2, 2)

        # 3. DRAW THE NODES
        for n in nodes:
            pygame.draw.circle(screen, ZEGA_GREEN, (int(n['x']), int(n['y'])), 4)
            if math.hypot(n['x']-STAR_POS[0], n['y']-STAR_POS[1]) < 85:
                world.spawn_particle(n['x'], n['y'], random.uniform(-5,5), random.uniform(-5,5))

        # 4. DRAW THE PARTICLES
        for trail, life in world.get_particles():
            if len(trail) > 1:
                pygame.draw.lines(screen, [int(255*life)]*3, False, trail, 1)

        pygame.display.flip()
        clock.tick(60)

if __name__ == "__main__": main()