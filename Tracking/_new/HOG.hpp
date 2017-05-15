#pragma once

using namespace cv;
using namespace std;

class MyHog {
public:

  cv::Mat Mag[2], AngleOf[2], Dx, Dy;

  struct BlockData
  {
    int histOfs;
    Point imgOffset;
  };
  struct PixelData
  {
    int gradOfs, qangleOfs;
    int histOfs[4];
    float histWeightOfs[4];
    float gradWeight;
  };

  Size mWinSize;
  Size mBlockSize;
  Size mBlockStride;
  Size mCellSize;
  Size nWindows;
  Size nBlocks;
  Size nCells;
  Point mWinOffset;
  int mBlockHistogramSize;
  vector<float> mHist;
  vector<BlockData> blockData;
  vector<PixelData> pixelData;
	
  int blockHistSize;
  int nBins;
  int count1, count2, count4;
  
  MyHog(cv::Mat& imgSrc);
  
  MyHog();
  
  ~MyHog();

  void init(Mat& imgSrc, Size winSize = Size(64, 128), Size blockSize = Size(16, 16),
	    Size blockStride = Size(8, 8), Size cellSize = Size(8, 8));
  
  void myGammaCorrection(cv::Mat& img, float fgamma);
  
  void computeGradient(const cv::Mat& imgSrc);
  
  const float* getHistOfBlock(Point imgOffset, float* dbuf);
  
  void normalizeBlockHistogram(float* _hist);
  
  void compute(const cv::Mat& imgSrc, vector<float>descriptor);

};


