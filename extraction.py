# -*- coding: utf-8 -*-
"""
Created on Tue May  1 13:03:35 2018

@author: ECHOES
"""

from skimage import io
import numpy

#1 prep

#1.1 load image1_grey.jpg as a grey pic
image1_grey = io.imread('test1.jpg', as_grey=True)
io.imshow(image1_grey)

#1.2 get pic size
height, width = image1_grey.shape
print('height, width', height, width)

#2 threshold: Vth = E + alpha * sigma

#2.1 E
E = image1_grey.mean()
"""
#naive alternative
E = 0
for i in range(0, height, 1):
    for j in range(0, width, 1):
        E += image1_grey[i, j]
E /= height * width
"""
print('E', E)

#2.2 alpha
alpha = 5
print('alpha', alpha)

#2.3 sigma
sigma = 0
for i in range(0, height, 1):
    for j in range(0, width, 1):
        sigma += numpy.square(image1_grey[i, j] - E)

sigma /= height * width - 1
sigma = numpy.sqrt(sigma)
print('sigma', sigma)

#2.4 Vth
Vth = E + alpha * sigma
print('Vth', Vth)

#3 Threshold segmentation
area = image1_grey[:, :] < Vth
image1_grey[area] = 0
"""
#naive alternative
for i in range(0, height, 1):
    for j in range(0, width, 1):
        if image1_grey[i, j] < Vth:
            image1_grey[i, j] = 0
"""                    
#io.imshow(image1_grey)
#io.imsave('test1_sem.jpg',image1_grey)

#4 x-y projection

#side * side each block
side = 5

#4.1 x projection

x_range = []
i = 0
while i < height:
    if sum(image1_grey[i, :]) > 0:
        up = i
        count = 0
        while count < side and i < height and sum(image1_grey[i, :]) > 0:
            i += 1
            count += 1
        down = i
        x_range.append([up, down])
    i += 1
    
print('x_range', x_range)

#4.2 y projection
y_range = []
i = 0
while i < width:
    if sum(image1_grey[:, i]) > 0:
        left = i
        count = 0
        while count < side and i < width and sum(image1_grey[:, i]) > 0:
            i += 1
            count += 1
        right = i
        y_range.append([left, right])
    i += 1

print('y_range', y_range)

#4.3 size
length_x = len(x_range)
length_y = len(y_range)
print('x_range, y_range', length_x, length_y)

#5 background threshold
T = Vth
print('T', T)

#6 centroid -> threshold centroid???
centroid = []

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
                if image1_grey[x + x_range[i][0], y + y_range[j][0]] >= T:
                    deno += image1_grey[x + x_range[i][0], y + y_range[j][0]]
                y += 1
            x += 1
        
        #---pure black block
        if deno == 0:
            j += 1
            continue
        
        #get x0's numerator
        numer = 0
        
        x = 1
        while x + x_range[i][0] < x_range[i][1]:
            y = 1
            while y + y_range[j][0] < y_range[j][1]:
                if image1_grey[x + x_range[i][0], y + y_range[j][0]] >= T:
                    numer += image1_grey[x + x_range[i][0], y + y_range[j][0]] * x
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
                if image1_grey[x + x_range[i][0], y + y_range[j][0]] >= T:
                    numer += image1_grey[x + x_range[i][0], y + y_range[j][0]] * y
                y += 1
            x += 1
                
        #get y0
        y0 = numer / deno
        
        #store [x_range[i][0] + x0, y_range[j][0] + y0]
        centroid.append([x_range[i][0] + x0, y_range[j][0] + y0])
        
        j += 1
    i += 1

#centroid count
length = len(centroid)
print('length', length)

#7 for verification
    
"""
#7.1 save the centroid as a list into a .npy file
numpy.save('centroid.npy', centroid)
"""

"""
#7.2 output the list into a file
#need to build an empty data.txt first
doc = open('data.txt', 'w')
print('centroid = [', centroid[0], ',', file = doc)
i = 1
while i < length - 1:
    print(centroid[i], ',', file = doc)
    i += 1
print(centroid[length - 1], ']', file = doc)
    
doc.close()
"""

"""
#7.3 output the data into a file
#need to build an empty data.txt first
doc = open('data.txt', 'w')
i = 0
while i < length:
    print(centroid[i][0], centroid[i][1], file = doc)
    i += 1
    
doc.close()
"""

#7.4 to connect to the next part
doc = open('centroid.txt', 'w')
i = 0
while i < length:
    print(centroid[i][0], centroid[i][1], file = doc)
    i += 1
    
doc.close()

