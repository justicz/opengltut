import pygame, math
import numpy as np
import colorsys
import random
import time

COLUMN_WIDTH = 50

LOOPRATE = 100 #lps

WINDOW_WIDTH  = 1920 #px
WINDOW_HEIGHT = 1080 #px

COLUMNS = WINDOW_WIDTH/COLUMN_WIDTH
MAX_COLUMN_HEIGHT = 400 #px

COLOR_EVOLUTION = .001

DASHPOT         = 0.
K               = 0.
NEIGHBOR_K = .002
DRIVER_OMEGA = 0.0005

class Column(object):
    def __init__(self, index, height, velocity, left=None, right=None):
        self.index = index
        self.height, self.velocity = height, velocity
        self.left, self.right = left, right

    def acceleration(self):
        own = self.height*K + self.velocity*DASHPOT
        self.left_force, self.right_force = 0., 0.
        if self.left:
            self.left_force = NEIGHBOR_K*(self.height - self.left.height)
        if self.right:
            self.right_force = NEIGHBOR_K*(self.height - self.right.height)
        return -(own + self.right_force + self.left_force)

    def step(self):
        self.height += self.velocity
        self.velocity += self.acceleration()
        
class Driver(object):
    def __init__(self):
        self.counter = 0.
        self.height = 0.
    def step(self):
        if self.counter < int(2 * 2*math.pi / DRIVER_OMEGA):
            self.height = MAX_COLUMN_HEIGHT*math.sin(DRIVER_OMEGA * self.counter)
            self.counter += 1
        
def main_loop():
    cs = []
    for i in xrange(COLUMNS):
        cs.append(Column(i, 0, 0.))
        if i != 0:
            cs[i].left = cs[i - 1]
            cs[i - 1].right = cs[i]
            
    driver = Driver()
    cs[COLUMNS - 1].right = driver

    step = 0
    while True:
        step += 1
        time.sleep(1./LOOPRATE)
        screen.fill((0, 0, 0))
        for c in cs:
            # set color based on height and time
            H = abs(c.height)/(MAX_COLUMN_HEIGHT*5.)
            color_evo = step*COLOR_EVOLUTION % 1
            color = colorsys.hsv_to_rgb(color_evo + H, 0.8, 0.8)
            color = [rgb*255. for rgb in color]

            pygame.draw.line(screen, color,
                            (c.index*COLUMN_WIDTH, WINDOW_HEIGHT/2.),
                            (c.index*COLUMN_WIDTH, WINDOW_HEIGHT/2. + c.height),
                            COLUMN_WIDTH)
            c.step()
            driver.step()
        pygame.display.update()
 
if __name__ == "__main__":
    pygame.init()
    screen = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
    main_loop()
