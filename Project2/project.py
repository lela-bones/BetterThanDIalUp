'''
High-Preformance Computing Class.
Description:
 
     With a triangle of unit area, let A, B, C denote the sides of the
     triangle. Let r, s, t denote 3 points randomly chosen along the
     respective sides s.t. r != s != t
 
     1. Find the probability distribution for the area of the inscribed
     triangles with |A_i - A_j| < err for n > 0 triangles.
     
     2. Think about P.D #1 how to enact Monte Carlo Simulation
 
AUTHORS: Matt Matuk, Lela Bones, Preston Mowe... "Spelling
'''

from mpi4py import MPI
import math
import random
import matplotlib.pyplot as plt
import numpy as np 

def sub_Tri():
    r = []
    s = []
    t = []
    while (r == t) and (t == s) and (s == t):
        r_1 = random.uniform(0, 1)
        r_2 = random.uniform(0, 1)
        r = [0, r_1, r_2]
        s_1 = random.uniform(0, 1)
        s_2 = random.uniform(0, 1)
        s = [s_1, 0, s_2]
        t_1 = random.uniform(0, 1)
        t_2 = random.uniform(0, 1)
        t = [t_1, t_2, 0]
    return r, s, t

def tri_area(tri):
    area = abs((tri[0][0] * (tri[1][1]-tri[2][1]) + tri[1][0] * (tri[2][1] - tri[0][1]) + tri[2][0] * (tri[0][1] - tri[1][1])) / 2)
    return area

def barry_to_cart(triangle, r, s, t):
    P_r_x = 0 + r[1] * triangle[1][0] + r[2] * triangle[2][0]
    P_r_y = 0 + r[1] * triangle[1][1] + r[2] * triangle[2][1]
    P_r = [P_r_x, P_r_y]
    print(P_r)
    P_s_x = s[0] * triangle[0][0] + 0 + s[2] * triangle[2][0]
    P_s_y = s[0] * triangle[0][1] + 0 + s[2] * triangle[2][1]
    P_s = [P_s_x, P_s_y]
    P_t_x = t[0] * triangle[0][0] + t[1] * triangle[1][0] + 0
    P_t_y = t[0] * triangle[0][1] + t[1] * triangle[1][1] + 0
    P_t = [P_t_x, P_t_y]
    return P_r, P_s, P_t
    
comm = MPI.COMM_WORLD
rank = comm.Get_rank()

if rank == 0:
    #initializing unit triangle
    triangle = np.array([[0,0], [2, 2], [1, 0]])
    r, s, t = sub_Tri()
    r, s, t = barry_to_cart(triangle, r, s, t)
    sub_tri = np.array([r, s, t])
    print(sub_tri)
    tri_area(sub_tri)
    plt.figure()
    plt.scatter(triangle[:, 0], triangle[:, 1])
    tri = plt.Polygon(triangle[:3, :])
    plt.gca().add_patch(tri)
    plt.scatter(sub_tri[:, 0], sub_tri[:, 1])
#     tri = plt.Polygon(sub_tri[:3, :])
#     plt.gca().add_patch(tri)
    plt.show()