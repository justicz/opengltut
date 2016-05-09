import pygame, math
import numpy as np
import random
import time

COLUMN_WIDTH = 12

FRAMERATE = 100 #fps

WINDOW_WIDTH  = 1920 #px
WINDOW_HEIGHT = 1080 #px

COLUMNS = WINDOW_WIDTH/COLUMN_WIDTH

DASHPOT         = 0.0001
K               = 0.0001
NEIGHBOR_K = .00001

class Column(object):
    def __init__(self, index, height, velocity, left=None, right=None):
        self.index = index
        self.height, self.velocity = height, velocity
        self.left, self.right = left, right

    def acceleration(self):
        own = self.height*K + self.velocity*DASHPOT
        left, right = 0., 0.
        if self.left:
            left = NEIGHBOR_K*(self.height - self.left.height)
        if self.right:
            right = NEIGHBOR_K*(self.height - self.right.height)
        return -(own + right + left)

    def step(self):
        self.height += self.velocity
        self.velocity += self.acceleration()

def main_loop():
    cs = []
    for i in xrange(COLUMNS):
        cs.append(Column(i, random.randint(-200, 200), 0.))
        if i != 0:
            cs[i].left = cs[i - 1]
            cs[i - 1].right = cs[i]

    while True:
        time.sleep(1./FRAMERATE)
        screen.fill((0, 0, 0))
        for c in cs:
            pygame.draw.line(screen, (155,0,255),
                            (c.index*COLUMN_WIDTH, WINDOW_HEIGHT/2.),
                            (c.index*COLUMN_WIDTH, WINDOW_HEIGHT/2. + c.height),
                            COLUMN_WIDTH)
            c.step()
        pygame.display.update()
 
if __name__ == "__main__":
    pygame.init()
    screen = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
    main_loop()
