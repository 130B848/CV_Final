#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <fstream>
//#include <opencv2/core.hpp>
//#include <opencv2/highgui.hpp>
//#include <opencv2/imgproc.hpp>
//#include <opencv2/features2d.hpp>
//#include <opencv2/nonfree.hpp>

#define BYTE unsigned char

using namespace cv;
using namespace std;
void goodMatch(double ratio, vector<vector<DMatch>> nMatches, vector<DMatch> &matches){
    for(unsigned int i = 0; i < nMatches.size(); i++)   //精确匹配
    {

        if(nMatches[i][0].distance < ratio * nMatches[i][1].distance)
            matches.push_back(nMatches[i][0]);
    }
}

int main(int argc, const char * argv[]) {
    // insert code here...
    Ptr<Feature2D> f2d = xfeatures2d::SIFT::create();

//    Mat img_1 = imread("/Users/googo/Desktop/123.png");
    Mat img_1 = imread(argv[1]);
    Mat img_2 = imread(argv[2]);
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
    double ratio = 0.75;
    while(true){
        goodMatch(ratio, nMatches, matches);
        vector<int> xpixel;
        vector<int> ypixel;
        for(auto m : matches){
            int x= int(keypoints_2[m.trainIdx].pt.y);
            int y= int(keypoints_2[m.trainIdx].pt.x);//(x, y)即是目标点在目标图片位置
            xpixel.push_back(x);
            ypixel.push_back(y);
        }
        int xMin, xMax, yMin ,yMax;
        if (xpixel.size() && ypixel.size()) {
            sort(xpixel.begin(), xpixel.end());
            sort(ypixel.begin(), ypixel.end());
            xMin = xpixel[0];
            xMax = xpixel[xpixel.size() - 1];
            yMin = ypixel[0];
            yMax = ypixel[ypixel.size() - 1];
        }
        double col = img_2.cols;
        // if((double)(yMax - yMin)/ col > 0.2){
        //     ratio = ratio * 0.95;
        //     matches.clear();
        //     continue;
        // }
        int xExpend = (yMax - yMin) * 0.5;
        int yExpend = img_2.rows * 0.05;
        xMin = xMin - yExpend < 0 ? 0 : xMin - yExpend;
        xMax = xMax + yExpend > img_2.rows ? img_2.rows: xMax + yExpend;
        yMin = yMin - xExpend < 0 ? 0 : yMin - xExpend;
        yMax = yMax + xExpend > img_2.cols ? img_2.cols: yMax + xExpend;
        Rect rec = Rect(yMin, xMin, yMax-yMin, xMax-xMin);
        Mat test = img_2(rec);
        // imshow("show", img_2);
        // waitKey();
        imwrite(argv[3], test);
        cout<<matches.size() <<"   "<< img_2.rows << "  over\n";
        break;
    }
   // waitKey();
    return 0;
}

// #include <iostream>
// #include <opencv2/opencv.hpp>
// #include <opencv2/xfeatures2d.hpp>
// #include <fstream>
// #include <algorithm>
// //#include <opencv2/core.hpp>
// //#include <opencv2/highgui.hpp>
// //#include <opencv2/imgproc.hpp>
// //#include <opencv2/features2d.hpp>
// //#include <opencv2/nonfree.hpp>
//
// #define BYTE unsigned char
//
// using namespace cv;
// using namespace std;
//
//
// int main(int argc, const char * argv[]) {
//     Ptr<Feature2D> f2d = xfeatures2d::SIFT::create();
//
//     Mat img_1 = imread(argv[1]); // img
//     Mat img_2 = imread(argv[2]); // template
//     cout << img_1.size() << ' ' << img_2.size() << endl;
//     //Detect the keypoints
//     vector<KeyPoint> keypoints_1, keypoints_2;
//     f2d->detect(img_1, keypoints_1);
//     f2d->detect(img_2, keypoints_2);
//     //Calculate descriptors (feature vectors)
//     Mat descriptors_1, descriptors_2;
//     f2d->compute(img_1, keypoints_1, descriptors_1);
//     f2d->compute(img_2, keypoints_2, descriptors_2);
//     //Matching descriptor vector using BFMatcher
//     BFMatcher matcher;
//     vector<vector<DMatch>> nMatches;
//     vector<DMatch> matches;
//     matcher.knnMatch(descriptors_1, descriptors_2, nMatches,2);
//     for(unsigned int i = 0; i < nMatches.size(); i++) {
//
//         if(nMatches[i][0].distance < 0.75 * nMatches[i][1].distance)
//             matches.push_back(nMatches[i][0]);
//     }
//     Mat img_matches;
//     vector<int> xpixel;
//     vector<int> ypixel;
//     for(auto m : matches){
//         int x = int(keypoints_2[m.trainIdx].pt.y);
//         int y = int(keypoints_2[m.trainIdx].pt.x);//(x, y)即是目标点在目标图片位置
//         xpixel.push_back(x);
//         ypixel.push_back(y);
//     }
//     cout << xpixel.size() << ' ' <<  ypixel.size() << endl;
//
//     if (xpixel.size() && ypixel.size()) {
//         sort(xpixel.begin(), xpixel.end());
//         sort(ypixel.begin(), ypixel.end());
//         int xMin = xpixel[0];
//         int xMax = xpixel[xpixel.size() - 1];
//         int yMin = ypixel[0];
//         int yMax = ypixel[ypixel.size() - 1];
//         cout << xMin << endl << yMin << endl << xMax << endl << yMax << endl;
//         Rect rec = Rect(yMin - 5, xMin - 5, yMax - yMin + 5, xMax - xMin + 5);
//         Mat test = img_2(rec);
//         //imshow("test", test);
//         //pt.close();
//         imwrite(argv[3], test);
//         cout << matches.size() << "  over\n";
//     }
//     return 0;
// }
