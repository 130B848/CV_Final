import cv2
import numpy as np

def dilation(inFile, outFile, dkernel, iterations):
        img = cv2.imread(inFile, cv2.IMREAD_GRAYSCALE)
        img = cv2.dilate(img, dkernel, iterations)
        cv2.imwrite(outFile, img)

def erosion(inFile, outFile, ekernel, iterations):
    img = cv2.imread(inFile, cv2.IMREAD_GRAYSCALE)
    img = cv2.erode(img, ekernel, iterations)
    cv2.imwrite(outFile, img)

#seHeight = input("Please input the height of structing element (pixel): ")
#seWidth = input("Please input the width of structing element (pixel): ")
se = np.array([[1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
               [0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
               [0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
               [0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
               [0, 0, 0, 0, 0, 0, 0, 0, 0, 1]], dtype=np.uint8)
dilation("IMG_3289.PNG", "dilation.png", cv2.getStructuringElement(cv2.MORPH_CROSS,
    (3, 5)), 1)
erosion("IMG_3289.PNG", "erosion.png", cv2.getStructuringElement(cv2.MORPH_CROSS,
    (10, 10)), 1)
#erosion(, seHeight, seWidth)
