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

def sub_Tri(tri):
    r = []
    s = []
    t = []
    while (r == t) and (t == s) and (s == t):
        #randomizes the first point
        r_1 = random.uniform(0, 1)
        #forces the points to add up to 1
        r_2 = 1 - r_1
        r = [0, r_1, r_2]
   
        #randomizes the first point
        s_1 = random.uniform(0, 1)
        #forces the points to add up to 1
        s_2 = 1 - s_1
        
        #randomizes the first point
        t_1 = random.uniform(0, 1)
        #forces the points to add up to 1
        t_2 = 1 - t_1
        
        r = [0, r_1, r_2]
        s = [s_1, 0, s_2]
        t = [t_1, t_2, 0]
        
        r, s, t = barry_to_cart(tri, r, s, t)
        sub_tri = np.array([r, s, t])
        
    return sub_tri

def tri_area(tri):
    area = abs((tri[0][0] * (tri[1][1]-tri[2][1]) + tri[1][0] * (tri[2][1] - tri[0][1]) + tri[2][0] * (tri[0][1] - tri[1][1])) / 2)
    return area

def barry_to_cart(triangle, r, s, t):
    #caluculates r_x and r_y
    P_r_x = (r[0] * triangle[0][0]) + (r[1] * triangle[1][0]) + (r[2] * triangle[2][0])
    P_r_y = (r[0] * triangle[0][1]) + (r[1] * triangle[1][1]) + (r[2] * triangle[2][1])
    
    #calculates s_x and s_y
    P_s_x = (s[0] * triangle[0][0]) + (s[1] * triangle[1][0]) + (s[2] * triangle[2][0])
    P_s_y = (s[0] * triangle[0][1]) + (s[1] * triangle[1][1]) + (s[2] * triangle[2][1])
    
    #calculates t_x and t_y
    P_t_x = (t[0] * triangle[0][0]) + (t[1] * triangle[1][0]) + (t[2] * triangle[2][0])
    P_t_y = (t[0] * triangle[0][1]) + (t[1] * triangle[1][1]) + (t[2] * triangle[2][1])
    
    P_r = [P_r_x, P_r_y]
    P_s = [P_s_x, P_s_y]
    P_t = [P_t_x, P_t_y]
#     print(P_r, P_s, P_t)
    return P_r, P_s, P_t
    
triangle = None
num = None

comm = MPI.COMM_WORLD
rank = comm.Get_rank()

if rank == 0:
    #initializing unit triangle
    triangle = np.array([[0,0], [2, 2], [1, 0]])
    #print(tri_area(triangle))  
    print("===========================================================================")
    print("Running %d parallel MPI processes" % comm.size)
    num = int(input('How many subtriangles would you like to make per process?'))
else:
    data = None
    num = None

#sending triangle and num to all nodes
triangle = comm.bcast(triangle, root=0)
num = comm.bcast(num, root=0)

#initializing list to store areas
areas = []
i = 0
while (i < num):
    #creating a sub triangle
    sub_tri = sub_Tri(triangle)
    #calculating area and addint it to the areas list
    areas.append(tri_area(sub_tri))
    i = i + 1
areas = np.array(areas)

recv_area = None
if rank == 0:
    #allocating memory for the total area list
    recv_area = np.empty(num * comm.size, dtype = 'd')

#combining all of the lists together
comm.Gather(areas, recv_area, root = 0)

if rank == 0:
    #creating a frequency table of areas
    unique, counts = np.unique(recv_area, return_counts = True)
    #setting the error to be .001
    unique = np.round(unique, decimals = 3)
    

#allocating space for the probability distribution
pdt = np.empty(num, dtype = 'd')
comm.Scatter(recv_area, pdt, root=0)


for prob in pdt:
    pdt[1] = pdt[1] / (num * comm.size)
    
# comm.Gather()
#     sub_tri = sub_Tri(triangle)
#     plt.figure()
#     plt.scatter(triangle[:, 0], triangle[:, 1])
#     tri = plt.Polygon(triangle[:3, :])
#     plt.gca().add_patch(tri)
#     plt.scatter(sub_tri[:, 0], sub_tri[:, 1])
#     plt.show()