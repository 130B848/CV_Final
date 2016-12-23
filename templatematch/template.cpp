#include "opencv2/opencv.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <math.h>

#include <vector>

using namespace std;

bool
intersects(const vector<cv::Rect>& taken, cv::Rect r)
{
  int x1 = r.x;
  int y1 = r.y;
  int x2 = r.x + r.width;
  int y2 = r.y + r.height;
  int xx1, xx2, yy1, yy2;
  for (auto iter = taken.begin(); iter != taken.end(); iter++) {
    xx1 = iter->x;
    xx2 = iter->x + iter->width;
    yy1 = iter->y;
    yy2 = iter->y + iter->height;
    bool xin =
      ((xx1 - 2 < x1) && (x1 < xx2 + 2)) || ((xx1 - 2 < x2) && (x2 < xx2 + 2));
    bool yin =
      ((yy1 - 2 < y1) && (y1 < yy2 + 2)) || ((yy1 - 2 < y2) && (y2 < yy2 + 2));
    if (xin && yin) {
      return true;
    }
  }
  return false;
}

int
main(int argc, char** argv)
{
  const double kThreshold = 0.5;
  cv::Mat img = cv::imread(argv[1]);
  cv::Mat tpl = cv::imread(argv[2]);

  cv::Mat gimg, gtpl;
  cv::cvtColor(img, gimg, CV_BGR2GRAY);
  cv::cvtColor(tpl, gtpl, CV_BGR2GRAY);

  cv::Mat res(img.rows - tpl.rows + 1, img.cols - tpl.cols + 1, CV_32FC1);
  cv::matchTemplate(gimg, gtpl, res, CV_TM_CCOEFF_NORMED);
  cv::threshold(res, res, kThreshold, 1., CV_THRESH_TOZERO);

  vector<cv::Rect> taken;
  while (true) {
    double minval, maxval, threshold = kThreshold;
    cv::Point minloc, maxloc;
    cv::minMaxLoc(res, &minval, &maxval, &minloc, &maxloc);

    if (maxval >= threshold) {
      auto ur = cv::Point(maxloc.x + tpl.cols, maxloc.y + tpl.rows);
      auto rect =
        cv::Rect(maxloc.x, maxloc.y, tpl.cols, tpl.rows);
      if (!intersects(taken, rect)) {
        cv::rectangle(img, maxloc, ur, CV_RGB(0, 255, 0), 2);
        taken.push_back(cv::Rect(maxloc.x, maxloc.y, tpl.cols,tpl.rows));
      }
      cv::floodFill(res, maxloc, cv::Scalar(0), 0, cv::Scalar(.1),
                    cv::Scalar(1.));
    } else {
      break;
    }
  }

  cv::imwrite("result.png", img);
}
