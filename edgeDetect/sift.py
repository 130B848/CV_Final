import cv2
import numpy as np
from matplotlib import pyplot as plt
def matchSift(imgPath1, imgPath2):
    sift = cv2.xfeatures2d.SIFT_create()

    img1 = cv2.imread(imgPath1, cv2.IMREAD_GRAYSCALE)
    img2 = cv2.imread(imgPath2, cv2.IMREAD_GRAYSCALE)

    orb = cv2.ORB_create();
    kp1, des1 = orb.detectAndCompute(img1, None)
    kp2, des2 = orb.detectAndCompute(img2, None)

    img1 = cv2.drawKeypoints(img1, kp1, img1)
    img2 = cv2.drawKeypoints(img2, kp2, img2)
   
    # cv2.imwrite("t1.png", img1)
    # cv2.imwrite("converted1.png", img2)
    
    cv2.imshow("img1", img1)
    cv2.imshow("img2", img2)

    bf = cv2.BFMatcher()
    matches = bf.knnMatch(des1, des2, k=2)
    
    good = []
    record = []
    for m, n in matches:
        if m.distance < 0.75 * n.distance:
            good.append([m])
            record.append([m, n])
            # print 'distance', 'imgIdx', 'queryIdx', 'trainIdx'
            # print m.distance, m.imgIdx, m.queryIdx, m.trainIdx
            # print n.distance, n.imgIdx, n.queryIdx, n.trainIdx
            # print 
    
    # for i in record[:20]:
    #     print 'distance', 'imgIdx', 'queryIdx', 'trainIdx'
    #     print i[0].distance, i[0].imgIdx, i[0].queryIdx, i[0].trainIdx
    #     print i[1].distance, i[1].imgIdx, i[1].queryIdx, i[1].trainIdx
    #     print 

    # for i in good[:10]:
    #     print i.
    img3 = cv2.drawMatchesKnn(img1, kp1, img2, kp2, good[:30], None, flags=2)
    
    # cv2.imwrite("match_result.png", img3)
    
    cv2.imshow("img3", img3)

    cv2.waitKey(0)
    cv2.destroyAllWindows()

matchSift("t1.jpg", "converted1.jpg")
