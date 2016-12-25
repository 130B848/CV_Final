#include "opencv2/opencv.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <math.h>
#include <numeric>

#include <vector>

using namespace std;
using namespace cv;

class Intersector
{
  public:
  Intersector() = delete;

  Intersector(int row, int col, int tolerance = 2)
    : R(row)
    , C(col)
    , T(tolerance)
  {
  }

  //
  // Optimized version. But try to be explicit.
  //
  // @returns: whether r intersects with any of taken
  bool operator()(const vector<cv::Point>& taken, cv::Point r)
  {
    int x1 = r.x;
    int y1 = r.y;
    for (auto iter = taken.begin(); iter != taken.end(); iter++) {
      int xx1 = iter->x;
      int yy1 = iter->y;
      bool xin = (xx1 - R - T < x1) && (x1 < xx1 + R + T);
      bool yin = (yy1 - C - T < y1) && (y1 < yy1 + C + T);
      if (xin && yin) {
        return true;
      }
    }
    return false;
  }

  private:
  const int R;
  const int C;
  const int T;
};

vector<cv::Point>
getEs(cv::Mat img, cv::Mat tpl, const double T)
{
  cv::Mat gimg, gtpl;
  cv::cvtColor(img, gimg, CV_BGR2GRAY);
  cv::cvtColor(tpl, gtpl, CV_BGR2GRAY);

  cv::Mat res(img.rows - tpl.rows + 1, img.cols - tpl.cols + 1, CV_32FC1);
  cv::matchTemplate(gimg, gtpl, res, CV_TM_CCOEFF_NORMED);
  cv::threshold(res, res, T, 1., CV_THRESH_TOZERO);

  vector<cv::Point> taken;
  Intersector intersects(tpl.rows, tpl.cols);

  while (true) {
    double minval, maxval;
    cv::Point minloc, maxloc;
    cv::minMaxLoc(res, &minval, &maxval, &minloc, &maxloc);

    if (maxval >= T) {
      if (!intersects(taken, maxloc)) {
        taken.push_back(maxloc);
      }
      cv::floodFill(res, maxloc, cv::Scalar(0), 0, cv::Scalar(.1),
                    cv::Scalar(1.));
    } else {
      break;
    }
  }
  return taken;
}

class Heuristor
{
  public:
  Heuristor() = delete;
  explicit Heuristor(const cv::Size h)
    : H(h)
    , val_(h)
  {
  }

  Size begin() { return val_ = H; }

  cv::Size next()
  {
    val_.width++;
    val_.height++;
    return val_;
  }

  bool end() { return val_.width < 5 * H.width; }

  private:
  const cv::Size H;
  cv::Size val_;
};

double
correlation(const vector<cv::Point>& x)
{
  const double N = x.size();

  cv::Point2d avgXY = std::accumulate(
    x.begin(), x.end(), cv::Point2d(0.0, 0.0),
    [](Point2d a, Point b) { return Point2d(a.x + b.x, a.y + b.y); });
  avgXY = avgXY / N;

  cv::Point2d sigmaXY = std::accumulate(
    x.begin(), x.end(), cv::Point2d(0.0, 0.0), [avgXY](Point2d a, Point b) {
      return Point2d(a.x + pow(b.x - avgXY.x, 2), a.y + pow(b.y - avgXY.y, 2));
    });

  sigmaXY = Point2d(sqrt(sigmaXY.x / N), sqrt(sigmaXY.y / N));

  // r(correlation) = sum(xi-avgx)*sum(yi-avgy) /
  // sqrt(sum((xi-avgx)^2))*sqrt(sum((yi-avgy)^2))
  double r = std::accumulate(x.begin(), x.end(), 0.0,
                             [avgXY, sigmaXY](double a, Point b) // plus
                             {
                               return a + ((b.x * 1.0 - avgXY.x) / sigmaXY.x *
                                           (b.y * 1.0 - avgXY.y) / sigmaXY.y);
                             });

  return abs(r / N);
  // return numerator / denominator;
}

const int MAX_E = 20;
const int MIN_E = 4;
const double MIN_R = 0.9; // ignore all correlation smaller than this.
// predict whether match1 is better than match2.
double
better(double r1, size_t sz1, double r2, size_t sz2)
{
  if (r1 < MIN_R) return false;
  if (sz1 < MIN_E || sz1 > MAX_E) return false;

  if (r1 > r2 + 0.01) return true; // great advance
  if (r1 < r2 - 0.05)
    return false;   // correlation is important. 99% better than 95%
  return sz1 > sz2; // simply compare size
}

int
main(int argc, char** argv)
{
  // Parameters
  const double kMaxThreshold = 0.7;
  const double kMinThreshold = 0.4;
  cv::Mat img = cv::imread(argv[1]);
  cv::Mat tpl = cv::imread(argv[2]);

  Mat tplflip;
  flip(tpl, tplflip, 0 /* flip around x-axis */);

  const cv::Size heursize = cv::Size(stof(argv[3]), stof(argv[4]));
  cout << "heuristic size of E: " << heursize << endl;

  // Search from big threshold so we stop at best possible threshold. Bestr < 0:
  // no good linear match.
  vector<cv::Point> best;
  cv::Size bestsz;
  double bestr = -1;

  // flipped version
  vector<cv::Point> bestf;
  cv::Size bestszf;
  double bestrf = -1;
  auto heuristor = Heuristor(heursize);

  vector<Mat> tpls, ftpls;
  vector<Size> sizes;
  for (cv::Size h = heuristor.begin(); heuristor.end();
       h = heuristor.next()) { // try different size
    cv::Mat tmptpl, tmptplf;
    cv::resize(tpl, tmptpl, h, CV_INTER_AREA);
    cv::resize(tplflip, tmptplf, h, CV_INTER_AREA);
    tpls.push_back(tmptpl);
    ftpls.push_back(tmptplf);
    sizes.push_back(h);
  }

  for (double threshold = kMaxThreshold; threshold > kMinThreshold && bestr < 0;
       threshold -= 0.05) {
    for (int i = 0; i < tpls.size(); i++) {
      vector<cv::Point> points = getEs(img, tpls[i], threshold);
      vector<cv::Point> pointsf = getEs(img, ftpls[i], threshold);
      double r = correlation(points);
      double rf = correlation(pointsf);
      // cout << " h=" << sizes[i] << " found " << points.size()
      //     << " correlation = " << r << endl;
      if (better(r, points.size(), bestr, best.size())) {
        best = std::move(points);
        bestsz = sizes[i];
        bestr = r;
      } else if (better(rf, pointsf.size(), bestr, best.size())) {
        best = std::move(pointsf);
        bestsz = sizes[i];
        bestr = rf;
      }
    }

    if (bestr > 0) {
      cout << "Find " << best.size() << " E. Best template size is " << bestsz
           << " threshold=" << threshold << endl;
      cout << " correlation = " << bestr << endl;
      for (auto r : best) {
        cv::rectangle(img, r,
                      cv::Point(r.x + bestsz.width, r.y + bestsz.height),
                      CV_RGB(0, 255, 255), 2);
      }
      break;
    } else {
      cout << "NO MATCH FOR THRESHOLD=" << threshold << endl;
    }
  }

  // OpenCV's fitline. Can't return goodness of the fit.
  //
  // cv::Vec4f line;
  // cv::fitLine(best, line, CV_DIST_L2 /* least-squares method */, 0, 0.01,
  // 0.01);
  // cout << line << endl;
  // cv::line(img, cv::Point(line[2] - 10 * line[0], line[3] - 10 * line[0]),
  //         cv::Point(line[3] + 20 * line[0], line[1] + 20 * line[3]),
  //         CV_RGB(0, 255, 0), 5);

  cv::imwrite(string(argv[5]) + ".png", img);
}
