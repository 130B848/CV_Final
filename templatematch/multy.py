import cv2
import numpy as np
from matplotlib import pyplot as plt
import sys

def intersects(taken, dl, ur): # d(own)l(eft), u(p)r(ight)
    x1,y1=dl
    x2,y2=ur
    for tdl, tur in taken:
        xx1,yy1=tdl
        xx2,yy2=tur
        xin = xx1-2 < x1 < xx2+2 or xx1-2 < x2 < xx2+2
        yin = yy1-2 < y1 < yy2+2 or yy1-2 < y2 < yy2+2
        if xin and yin :
            return True
        #print "not match", dl, ur, tdl, tur
    return False

img_rgb = cv2.imread(sys.argv[1])
img_gray = cv2.cvtColor(img_rgb, cv2.COLOR_BGR2GRAY)
template = cv2.imread(sys.argv[2],0)
w, h = template.shape[::-1]
res = cv2.matchTemplate(img_gray,template,cv2.TM_CCOEFF_NORMED)
threshold = 0.58 #0.58 for e1.jpg 0.7 for e0print.jpg/e2.jpg
loc = np.where( res >= threshold)

taken = []

for pt in zip(*loc[::-1]):
    corner = (pt[0]+w, pt[1]+h)
    if not intersects(taken, pt, corner):
        cv2.rectangle(img_rgb, pt, corner, (0,0,255), 2)
        taken.append((pt,corner))

cv2.imwrite('res.png',img_rgb)
