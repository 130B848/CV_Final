#!/usr/bin/env python
import cv2
import numpy as np
from matplotlib import pyplot as plt

img = cv2.imread("t1.jpg", cv2.IMREAD_GRAYSCALE)
# blur = cv2.bilateralFilter(img, 15, 100, 100)
edges = cv2.Canny(img, 50, 150, apertureSize = 3)

lines = cv2.HoughLinesP(edges, 1, np.pi / 180, 100, 100, 10)
for line in lines:
    x1, y1, x2, y2 = line[0]
    cv2.line(img, (x1, y1), (x2, y2), (0, 255, 0), 2)

cv2.imwrite("canny.png", img)
