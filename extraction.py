# -*- coding: utf-8 -*-
"""
Created on Tue May  1 13:03:35 2018

@author: ECHOES
"""

from scikit-image import *
import numpy

"""
#image0 grey map
image0_grey = io.imread('C:/Users/ECHOES/Desktop/test0.jpg', as_grey=True)
io.imshow(image0_grey)
"""

#1 image1_grey.jpg
image1_grey = io.imread('C:\Users\70700\Desktop\Skymap\1.jpg', as_grey=True)
#io.imshow(image1_grey)
height, width = image1_grey.shape
print('height', height)
print('width', width)

#2 Vth = E + alpha * sigma
#2.1 E
E = image1_grey.mean()
print('E', E)

"""
#alternative
E = 0
for i in range(0, height, 1):
    for j in range(0, width, 1):
        E += image1_grey[i, j]
E /= height*width
print('E', E)
"""

#2.2 alpha
alpha = 5
print('alpha', alpha)

#2.3 sigma
sigma=0
for i in range(0, height, 1):
    for j in range(0, width, 1):
        sigma += numpy.square(image1_grey[i, j] - E)

sigma /= height * width - 1
sigma = numpy.sqrt(sigma)
print('sigma', sigma)

#2.4 Vth
Vth = Vth = E + alpha * sigma
print('Vth', Vth)

"""
print('max',image1_grey.max())
print('min',image1_grey.min())
"""

#3 Threshold segmentation
for i in range(0, height, 1):
    for j in range(0, width, 1):
        if image1_grey[i, j] < Vth:
            image1_grey[i, j] = 0
#io.imshow(image1_grey)
#io.imsave('C:/Users/ECHOES/Desktop/extraction/test1_sem.jpg',image1_grey)

"""
print('max',image1_grey.max())
print('min',image1_grey.min())
"""

#4 x-y projection
x_range = []
i = 0
while i < height:
    if sum(image1_grey[i, :]) > 0:
        up = i
        while sum(image1_grey[i, :]) > 0 and i < height - 1:
            i += 1
        down = i
        x_range.append([up, down])
    i += 1

y_range = []
i = 0
while i < width:
    if sum(image1_grey[:, i]) > 0:
        left = i
        while sum(image1_grey[:, i]) > 0 and i < width - 1:
            i += 1
        right = i
        y_range.append([left, right])
    i += 1

print(x_range)
print(y_range)

#5 background threshold???
T = Vth
print('T', T)

"""
#6 centroid with threshold???
centroid = []

length_x = len(x_range)
length_y = len(y_range)

print('x_range', len(x_range))
print('y_range', len(y_range))

i = 0
while i < length_x:
    j = 0
    while j < length_y:
        
        #get denominator
        deno = 0
        x = 1
        while x + x_range[i][0] < x_range[i][1]:
            y = 1
            while y + y_range[j][0] < y_range[j][1]:
                if image1_grey[x + x_range[i][0]][y + y_range[j][0]] >= T:
                    deno += image1_grey[x + x_range[i][0]][y + y_range[j][0]] - T
                else:
                    deno -= T
                #deno += image1_grey[x + x_range[i][0]][y + y_range[j][0]] - T
                y += 1
            x += 1
        
        #get x0's numerator
        numer = 0
        
        x = 1
        while x + x_range[i][0] < x_range[i][1]:
            y = 1
            while y + y_range[j][0] < y_range[j][1]:
                if image1_grey[x + x_range[i][0]][y + y_range[j][0]] >= T:
                    numer += (image1_grey[x + x_range[i][0]][y + y_range[j][0]] - T) * x
                else:
                    numer -= T * x
                #numer += (image1_grey[x + x_range[i][0]][y + y_range[j][0]] - T) * x
                y += 1
            x += 1
            
        #get x0
        x0 = numer / deno
        
        #get y0's numberator
        numer = 0
        
        x = 1
        while x + x_range[i][0] < x_range[i][1]:
            y = 1
            while y + y_range[j][0] < y_range[j][1]:
                if image1_grey[x + x_range[i][0]][y + y_range[j][0]] >= T:
                    numer += (image1_grey[x + x_range[i][0]][y + y_range[j][0]] - T) * y
                else:
                    numer -= T * y
                #numer += (image1_grey[x + x_range[i][0]][y + y_range[j][0]] - T) * y
                y += 1
            x += 1
                
        #get y0
        y0 = numer / deno
        
        #store [x_range[i][0] + x0, y_range[j][0] + y0]
        centroid.append([x_range[i][0] + x0, y_range[j][0] + y0])
                
        j += 1
    i += 1

print(centroid)

"""


#6 centroid
centroid = []

length_x = len(x_range)
length_y = len(y_range)

print('x_range', len(x_range))
print('y_range', len(y_range))


i = 0
while i < length_x:
    j = 0
    while j < length_y:
        
        #get denominator
        deno = 0
        
        x =  1
        while x + x_range[i][0] < x_range[i][1]:
            y = 1
            while y + y_range[j][0] < y_range[j][1]:
                if image1_grey[x + x_range[i][0]][y + y_range[j][0]] >= T:
                    deno += image1_grey[x + x_range[i][0]][y + y_range[j][0]]
                y += 1
            x += 1
        
        if deno == 0:
            j += 1
            continue
        
        #get x0's numerator
        numer = 0
        
        x = 1
        while x + x_range[i][0] < x_range[i][1]:
            y = 1
            while y + y_range[j][0] < y_range[j][1]:
                if image1_grey[x + x_range[i][0]][y + y_range[j][0]] >= T:
                    numer += image1_grey[x + x_range[i][0]][y + y_range[j][0]] * x
                y += 1
            x += 1
            
        #get x0
        x0 = numer / deno
        
        #get y0's numberator
        numer = 0
        
        x = 1
        while x + x_range[i][0] < x_range[i][1]:
            y = 1
            while y + y_range[j][0] < y_range[j][1]:
                if image1_grey[x + x_range[i][0]][y + y_range[j][0]] >= T:
                    numer += image1_grey[x + x_range[i][0]][y + y_range[j][0]] * y
                y += 1
            x += 1
                
        #get y0
        y0 = numer / deno
        
        #store [x_range[i][0] + x0, y_range[j][0] + y0]
        centroid.append([x_range[i][0] + x0, y_range[j][0] + y0])
        print('centroid', x_range[i][0] + x0, y_range[j][0] + y0)
        j += 1
    i += 1

print(centroid)



