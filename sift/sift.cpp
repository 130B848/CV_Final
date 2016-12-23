//
//  main.cpp
//  SIFT
//
//  Created by 熊果 on 11/12/2016.
//  Copyright © 2016 熊果. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <fstream>
#include <algorithm>
//#include <opencv2/core.hpp>
//#include <opencv2/highgui.hpp>
//#include <opencv2/imgproc.hpp>
//#include <opencv2/features2d.hpp>
//#include <opencv2/nonfree.hpp>

#define BYTE unsigned char

using namespace cv;
using namespace std;


int main(int argc, const char * argv[]) {
    Ptr<Feature2D> f2d = xfeatures2d::SIFT::create();
  
    Mat img_1 = imread("/home/marklee/CV_Final/edgeDetect/t1.jpg");
    Mat img_2 = imread("/home/marklee/CV_Final/edgeDetect/111.jpg");
    cout << img_1.size() << ' ' << img_2.size() << endl;
    //Detect the keypoints
    vector<KeyPoint> keypoints_1, keypoints_2;
    f2d->detect(img_1, keypoints_1);
    f2d->detect(img_2, keypoints_2);
    //Calculate descriptors (feature vectors)
    Mat descriptors_1, descriptors_2;
    f2d->compute(img_1, keypoints_1, descriptors_1);
    f2d->compute(img_2, keypoints_2, descriptors_2);
    //Matching descriptor vector using BFMatcher
    BFMatcher matcher;
    vector<vector<DMatch>> nMatches;
    vector<DMatch> matches;
    matcher.knnMatch(descriptors_1, descriptors_2, nMatches,2);
    for(unsigned int i = 0; i < nMatches.size(); i++)   //精确匹配
    {
        
        if(nMatches[i][0].distance < 0.75 * nMatches[i][1].distance)
            matches.push_back(nMatches[i][0]);
    }
    Mat img_matches;
    vector<int> xpixel;
    vector<int> ypixel;
    for(auto m : matches){
        int x = int(keypoints_2[m.trainIdx].pt.y);
        int y = int(keypoints_2[m.trainIdx].pt.x);//(x, y)即是目标点在目标图片位置
        xpixel.push_back(x);
        ypixel.push_back(y);
    }
    cout << xpixel.size() << ' ' <<  ypixel.size() << endl;
    
    if (xpixel.size() && ypixel.size()) {
        sort(xpixel.begin(), xpixel.end());
        sort(ypixel.begin(), ypixel.end());
        int xMin = xpixel[0];
        int xMax = xpixel[xpixel.size() - 1];
        int yMin = ypixel[0];
        int yMax = ypixel[ypixel.size() - 1];
        cout << xMin << endl << yMin << endl << xMax << endl << yMax << endl;
        Rect rec = Rect(yMin, xMin, yMax-yMin, xMax-xMin);
        Mat test = img_2(rec);
        //imshow("test", test);
        //pt.close();
        imwrite("/home/marklee/CV_Final/result.jpg", test);
        cout << matches.size() << "  over\n";
    }
    return 0;
}

// fstream pt;
//    fstream k1, k2, ma;
//    k1.open("/Users/googo/Desktop/cv/A8 SIFT/SIFT/SIFT/k1.txt");
//
//
//    for (int i = 0; i < keypoints_1.size(); i ++) {
//        k1 << i << ": " << keypoints_1[i].pt << endl;
//    }
//    k1.close();
//    cout << "k1 over\n";
//    k2.open("/Users/googo/Desktop/cv/A8 SIFT/SIFT/SIFT/k2.txt");
//    for (int i = 0; i < keypoints_2.size(); i ++) {
//        k2 << i << ": " << keypoints_2[i].pt << endl;
//    }
//    k2.close();
//    cout << "k2 over\n";
//    ma.open("/Users/googo/Desktop/cv/A8 SIFT/SIFT/SIFT/m.txt");
//    for (int m = 0; m < matches.size(); m ++){
//        if(matches[m].distance > 100){
//            vector<KeyPoint>::iterator it = keypoints_1.begin();
//            it += matches[m].queryIdx;
//            keypoints_1.erase(it);
//
//            it = keypoints_2.begin();
//            it += matches[m].trainIdx;
//            keypoints_2.erase(it);
//
//            vector<DMatch>::iterator mit = matches.begin();
//            mit += m;
//            matches.erase(mit);
//
//        }
////        ma << m << ":" << matches[m].queryIdx << "; " << matches[m].trainIdx
////        <<"; "<<matches[m].imgIdx << "; " << matches[m].distance << endl;
//    }
//    ma.close();
// cout<<"over\n";

// drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, img_matches);
//    imshow("match", img_matches);
//    imwrite("/Users/googo/Desktop/cv/A8 SIFT/SIFT/SIFT/xgsb.jpg", img_matches);
// Mat test = img_2.clone();//拷贝目标图片
//vector<Point2f> points;

