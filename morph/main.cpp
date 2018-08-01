
#include "stdio.h"
#include <OpenCV/OpenCV.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace std;

int lrx, lry;
vector<CvPoint> allPoints;

CvMat *fundMat;
CvMat *points1;
CvMat *points2;

int counter = 16;
float alpha = 0;

int calcFundamental() {

  if (allPoints.size() != 16)
    return -1;

  points1 = cvCreateMat(2, allPoints.size() / 2, CV_32F);
  points2 = cvCreateMat(2, allPoints.size() / 2, CV_32F);
  CvMat *status = cvCreateMat(1, allPoints.size() / 2, CV_32F);

  for (int j = 0; j < allPoints.size(); j++) {

    if (j % 2 == 0) {
      cout << allPoints.at(j).x << ' ' << allPoints.at(j).y << endl;
      cvSetReal2D(points1, 0, j - (int)round(j / 2), allPoints.at(j).x);
      cvSetReal2D(points1, 1, j - (int)round(j / 2), allPoints.at(j).y);
    } else {
      cout << allPoints.at(j).x << ' ' << allPoints.at(j).y << endl;
      cvSetReal2D(points2, 0, j - (int)round(j / 2) - 1, allPoints.at(j).x);
      cvSetReal2D(points2, 1, j - (int)round(j / 2) - 1, allPoints.at(j).y);
    }
  }

  fundMat = cvCreateMat(3, 3, CV_32F);

  int num = cvFindFundamentalMat(points1, points2, fundMat, CV_FM_8POINT, 1.0,
                                 0.9999, status);

  return num;
}

void morph() {
  cout << calcFundamental() << endl;

  CvMat *epilines1 = cvCreateMat(3, 8, CV_32F);
  CvMat *epilines2 = cvCreateMat(3, 8, CV_32F);

  cvComputeCorrespondEpilines(points1, 1, fundMat, epilines1);
  cvComputeCorrespondEpilines(points2, 2, fundMat, epilines2);

  int lineCount;

  static CvMatrix3 matrix;

  float m00 = cvmGet(fundMat, 0, 0);
  float m01 = cvmGet(fundMat, 0, 1);
  float m02 = cvmGet(fundMat, 0, 2);
  float m10 = cvmGet(fundMat, 1, 0);
  float m11 = cvmGet(fundMat, 1, 1);
  float m12 = cvmGet(fundMat, 1, 2);
  float m20 = cvmGet(fundMat, 2, 0);
  float m21 = cvmGet(fundMat, 2, 1);
  float m22 = cvmGet(fundMat, 2, 2);

  matrix.m[0][0] = m00;
  matrix.m[0][1] = m01;
  matrix.m[0][2] = m02;
  matrix.m[1][0] = m10;
  matrix.m[1][1] = m11;
  matrix.m[1][2] = m12;
  matrix.m[2][0] = m20;
  matrix.m[2][1] = m21;
  matrix.m[2][2] = m22;

  CvMatrix3 *matScan = &matrix;

  cvMakeScanlines(matScan, cvSize(leftImage->width, leftImage->height), 0, 0, 0,
                  0, &lineCount);

  cout << lineCount << endl;

  int *lengthEpilines1 = new int[lineCount];
  int *lengthEpilines2 = new int[lineCount];

  int *epilinesInt1 = new int[4 * lineCount];
  int *epilinesInt2 = new int[4 * lineCount];

  cvMakeScanlines(matScan, cvSize(leftImage->width, leftImage->height),
                  epilinesInt1, epilinesInt2, lengthEpilines1, lengthEpilines2,
                  &lineCount);

  uchar *preWarpData1 =
      new uchar[max(leftImage->width, leftImage->height) * lineCount * 3];
  uchar *preWarpData2 =
      new uchar[max(leftImage->width, leftImage->height) * lineCount * 3];

  cout << "warping " << endl;

  cvPreWarpImage(lineCount, leftImage, preWarpData1, lengthEpilines1,
                 epilinesInt1);
  cvPreWarpImage(lineCount, rightImage, preWarpData2, lengthEpilines2,
                 epilinesInt2);

  cout << "done " << endl;

  int *numRuns1 = new int[lineCount];
  int *numRuns2 = new int[lineCount];

  int *runs1 = new int[leftImage->width * lineCount];
  int *runs2 = new int[leftImage->width * lineCount];

  int *runCorrelation1 =
      new int[max(leftImage->width, leftImage->height) * lineCount * 3];
  int *runCorrelation2 =
      new int[max(leftImage->width, leftImage->height) * lineCount * 3];

  cvFindRuns(lineCount, preWarpData1, preWarpData2, lengthEpilines1,
             lengthEpilines2, runs1, runs2, numRuns1, numRuns2);

  int *scanlinesMorphedImage = new int[lineCount * 2 * 4];
  int *numScanlinesMorphedImage = new int[lineCount * 2 * 4];

  cout << "runs " << endl;

  cvDynamicCorrespondMulti(lineCount, runs1, numRuns1, runs2, numRuns2,
                           runCorrelation1, runCorrelation2);

  cout << "dyn " << endl;

  uchar *tmpDataImageDst =
      new uchar[max(leftImage->width, leftImage->height) * lineCount * 3];

  int *scanlinesA = new int[lineCount * 2 * 4];
  int *lenghts = new int[lineCount * 2 * 4];

  cvMakeAlphaScanlines(epilinesInt1, epilinesInt2, scanlinesMorphedImage,
                       numScanlinesMorphedImage, lineCount, alpha);

  cvMorphEpilinesMulti(lineCount, preWarpData1, lengthEpilines1, preWarpData2,
                       lengthEpilines2, tmpDataImageDst,
                       numScanlinesMorphedImage, alpha, runs1, numRuns1, runs2,
                       numRuns2, runCorrelation1, runCorrelation2);

  cout << "morph " << endl;

  morphImage = cvCreateImage(cvSize(leftImage->width, leftImage->height), 8, 3);

  cvPostWarpImage(lineCount, tmpDataImageDst, numScanlinesMorphedImage,
                  morphImage, scanlinesMorphedImage);

  cvDeleteMoire(morphImage);

  cvNamedWindow("altro", 1);
  cvShowImage("altro", morphImage);

  cout << "alpha value: " << alpha << endl;
  alpha += 0.1;
}

int main(int argc, char **argv) {

  IplImage *morphImage, *leftImage, *rightImage;

  leftImage = cvLoadImage("philipp1.jpg");
  rightImage = cvLoadImage("philipp2.jpg");

  lrx = 0;
  lry = 0;

  while (1) {

    int i = 0;

    while (i < allPoints.size()) {
      CvScalar s =
          cvScalar(0 + rand() % 255, 0 + rand() % 255, 0 + rand() % 255);
      if (i % 2 == 0) {
        CvPoint tmpl = cvPoint(allPoints.at(i).x, allPoints.at(i).y);
        i++;
        cvCircle(leftImage, tmpl, 2, s, 2);
      } else {
        CvPoint tmpr = cvPoint(allPoints.at(i).x, allPoints.at(i).y);
        i++;
        cvCircle(rightImage, tmpr, 2, s, 2);
      }
    }
  }
}
