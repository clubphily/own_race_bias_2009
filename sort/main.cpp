#define CV_NO_BACKWARD_COMPATIBILITY

#include <OpenCV/OpenCV.h>

#include <fstream>
#include <iostream>

using namespace std;
using namespace cv;

void detectAndDraw(Mat &img, CascadeClassifier &cascade,
                   CascadeClassifier &nestedCascade, ofstream &outfile);

int main(int argc, const char **argv) {

  Mat image;
  const string amountOpt = "--amount=";
  size_t amountOptLen = amountOpt.length();
  string inputName;

  CascadeClassifier cascade, nestedCascade;
  int amount = 0;

  for (int i = 1; i < argc; i++) {
    if (amountOpt.compare(0, amountOptLen, argv[i], amountOptLen) == 0) {
      if (!sscanf(argv[i] + amountOpt.length(), "%d", &amount) || amount < 0) {
        amount = 0;
      }
    } else if (argv[i][0] == '-') {
      cerr << "WARNING: Unknown option %s" << argv[i] << endl;
    } else {
      inputName.assign(argv[i]);
    }
  }

  if (!cascade.load("haarcascade_frontalface_alt_tree.xml")) {
    cerr << "ERROR: Could not load classifier cascade" << endl;
    return -1;
  }
  if (!nestedCascade.load("haarcascade_mcs_righteye.xml")) {
    cerr << "WARNING: Could not load classifier cascade for nested objects"
         << endl;
    return -1;
  }
  ofstream outfile;
  outfile.open("test.csv");

  outfile << "name,left_eye_x,left_eye_y,right_eye_x,right_eye_y,face_x,face_y,"
             "face_width,face_height"
          << endl;

  for (int i = 1; i <= amount; i++) {

    stringstream out;
    out << inputName << i << ".jpg";
    outfile << out.str() << ",";
    image = imread(out.str(), 1);

    if (!image.empty()) {
      detectAndDraw(image, cascade, nestedCascade, outfile);
    }

    outfile << endl;
  }
  outfile.close();

  return 0;
}

void detectAndDraw(Mat &img, CascadeClassifier &cascade,
                   CascadeClassifier &nestedCascade,
                   ofstream &outfile) //, ofstream& meshfile)
{
  bool eyesFound = false, stop_searching = false;
  double t = 0;
  vector<Rect> faces;
  Mat gray, smallImg(cvRound(img.rows), cvRound(img.cols), CV_8UC1);

  cvtColor(img, gray, CV_BGR2GRAY);
  resize(gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR);
  equalizeHist(smallImg, smallImg);

  t = (double)cvGetTickCount();
  cascade.detectMultiScale(smallImg, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE,
                           Size(30, 30));
  for (vector<Rect>::const_iterator r = faces.begin(); r != faces.end(); r++) {
    Mat smallImgROI;
    vector<Rect> nestedObjects;
    vector<Point> centers;
    Point center, centerLeft, centerRight;

    smallImgROI = smallImg(*r);

    nestedCascade.detectMultiScale(smallImgROI, nestedObjects, 1.1, 2,
                                   0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

    for (vector<Rect>::const_iterator nr = nestedObjects.begin();
         nr != nestedObjects.end(); nr++) {
      center.x = cvRound((r->x + nr->x + nr->width * 0.5));
      center.y = cvRound((r->y + nr->y + nr->height * 0.5));

      if (center.y < (r->y + r->height * 0.5)) {
        centers.push_back(center);
      }
      eyesFound = true;
    }

    if (eyesFound) {
      int lefters = 0, righters = 0, xLeftSum = 0, xRightSum = 0, yLeftSum = 0,
          yRightSum = 0;
      for (vector<Point>::const_iterator cntr = centers.begin();
           cntr != centers.end(); cntr++) {
        if (cntr->x < (r->x + r->width * 0.5)) {
          lefters++;
          xLeftSum += cntr->x;
          yLeftSum += cntr->y;
        } else {
          righters++;
          xRightSum += cntr->x;
          yRightSum += cntr->y;
        }
      }

      if (lefters > 0 && righters > 0) {

        centerLeft.x = cvRound(xLeftSum / lefters);
        centerLeft.y = cvRound(yLeftSum / lefters);
        centerRight.x = cvRound(xRightSum / righters);
        centerRight.y = cvRound(yRightSum / righters);

        outfile << centerLeft.x << ",";
        outfile << centerLeft.y << ",";
        outfile << centerRight.x << ",";
        outfile << centerRight.y << ",";

        outfile << r->x << ",";
        outfile << r->y << ",";
        outfile << r->width << ",";
        outfile << r->height;

        stop_searching = true;
      }
    }
    centers.clear();
    eyesFound = false;
    if (stop_searching) {
      break;
    }
  }
  t = (double)cvGetTickCount() - t;
  printf("detection time = %g ms\n",
         t / ((double)cvGetTickFrequency() * 1000.));
}
