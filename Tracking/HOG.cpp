#include <iostream>
#include <fstream>

#include "HOG.hpp"

MyHog::MyHog(cv::Mat& imgSrc)
{
  init(imgSrc);
  mHist.resize(nWindows.area()*nBlocks.area()*nCells.area()*nBins);
}
  
MyHog::MyHog() {};
  
MyHog::~MyHog() {};

void MyHog::init(Mat& imgSrc, Size winSize, Size blockSize, Size blockStride, Size cellSize)
{

  Mat_<float> weights(blockSize);
  float sigma = (blockSize.height + blockSize.width) / 8;
  float scale = 1.f / (sigma*sigma * 2);
  for (int i = 0; i < blockSize.height; i++)
    for (int j = 0; j < blockSize.width; j++)
      {
	float di = i - blockSize.height*0.5f;
	float dj = j - blockSize.width*0.5f;
	weights(i, j) = std::exp(-(di*di + dj*dj)*scale);
      }

  //calculate the gradient and the angle
  void computeGradient(cv::Mat& imgSrc);

  mWinOffset = Point(0, 0);
  nBins = 9;
  mWinSize = winSize;
  mBlockSize = blockSize;
  mBlockStride = blockStride;
  mCellSize = cellSize;
  mBlockHistogramSize = 36;

  nWindows = Size( (imgSrc.cols - mWinSize.width) / mBlockStride.width + 1,
		   (imgSrc.rows - mWinSize.height) / mBlockStride.height + 1);
  nBlocks = Size(mWinSize.width / mBlockSize.width, mWinSize.height / mBlockSize.height);
  nCells = Size(mBlockSize.width / mCellSize.width, mBlockSize.height / mCellSize.height);

  blockHistSize = nBins*nCells.width*nCells.height;

  int rawBlockSize = mBlockSize.width*mBlockSize.height;
        
  blockData.resize(nBlocks.width*nBlocks.height);

  pixelData.resize(rawBlockSize * 3);
  count1 = count2 = count4 = 0;

  for (int i = 0; i < mBlockSize.width; i++)
    for (int j = 0; j < mBlockSize.height; j++)
      {
	PixelData* data = 0;

	float cellX = (i + 0.5f) / mCellSize.width - 0.5f;
	float cellY = (j + 0.5f) / mCellSize.height - 0.5f;

	int iCellX0 = cvFloor(cellX);
	int iCellY0 = cvFloor(cellY);
	int iCellX1 = iCellX0 + 1, iCellY1 = iCellY0 + 1;

	cellX -= iCellX0;
	cellY -= iCellY0;

	if (iCellX0 == 0) 
	  {
	    if (iCellY0 == 0)
	      {
		data = &pixelData[rawBlockSize * 2 + (count4++)];

		data->histOfs[0] = (iCellX0 * nCells.height + iCellY0)*nBins;
	       
		data->histWeightOfs[0] = (1.0f - cellX) * (1.0f - cellY);

		data->histOfs[1] = (iCellX1 * nCells.height + iCellY0)*nBins;
	       
		data->histWeightOfs[1] = cellX * (1.0 - cellY);

		data->histOfs[2] = (iCellX0 * nCells.height + iCellY1)*nBins;
		
		data->histWeightOfs[2] = (1.0f - cellX) *  cellY;

		data->histOfs[3] = (iCellX1 * nCells.height + iCellY1)*nBins;
		
		data->histWeightOfs[3] = cellX * cellY;
	      }
	    else
	      {
		data = &pixelData[rawBlockSize * 1 + (count2++)];
			
		if (iCellY0 == 1)
		  {
		    iCellY1 = iCellY0;
		    cellY = 1.0f - cellY;
		  }
		data->histOfs[0] = (iCellX0 * nCells.height + iCellY1)*nBins;
	       
		data->histWeightOfs[0] = (1.0f - cellX) * cellY;

		data->histOfs[1] = (iCellX1 * nCells.height + iCellY1)*nBins;
		
		data->histWeightOfs[1] = cellX * cellY;

		data->histOfs[2] = data->histOfs[3] = 0;
		
		data->histWeightOfs[2] = data->histWeightOfs[3] = 0;
	      }
	  }
	else
	  {	
	    if (iCellX0 == 1)
	      {
		iCellX1 = iCellX0;
		cellX = 1.0f - cellX;
	      }

	    if (iCellY0 == 0)
	      {
		data = &pixelData[rawBlockSize * 1 + (count2++)];

		data->histOfs[0] = (iCellX1 * nCells.height + iCellY0)*nBins;
		data->histWeightOfs[0] = cellX *  (1.0f - cellY);

		data->histOfs[1] = (iCellX1 * nCells.height + iCellY1) * nBins;
		data->histWeightOfs[1] = cellX * cellY;

		data->histOfs[2] = data->histOfs[3] = 0;
		data->histWeightOfs[2] = data->histWeightOfs[3] = 0;
	      }
	    else
	      {
		data = &pixelData[rawBlockSize * 0 + (count1++)];

		if (iCellY0 == 1)
		  {
		    iCellY1 = iCellY0;
		    cellY = 1.0f - cellY;
		  }

		data->histOfs[0] = (iCellX1*nCells.height + iCellY1)*nBins;
		data->histWeightOfs[0] = cellX*cellY;

		data->histOfs[1] = data->histOfs[2] = data->histOfs[3] = 0;
		data->histWeightOfs[1] = data->histWeightOfs[2] = data->histWeightOfs[3] = 0;

	      }
	  }

	data->gradOfs = i*Mag[0].cols + j;
	data->qangleOfs = i*AngleOf[0].cols+ j;
		        
	data->gradWeight = weights(i, j);
      }

  assert(count1 + count2 + count4 == rawBlockSize);

  for (int i = 0; i < count2; i++)
    {
      pixelData[i + count1] = pixelData[rawBlockSize + i];
    }
  for (int i = 0; i < count4; i++)
    {
      pixelData[i + count1 + count2] = pixelData[rawBlockSize * 2 + i];
    }

  int blockHistogramSize = 36;
  for (int i = 0; i < nBlocks.width; i++)
    {
      for (int j = 0; j < nBlocks.height; j++)
	{
	  BlockData* data = &blockData[i*nBlocks.height + j];

	  data->histOfs = (i*nBlocks.height + j)*blockHistogramSize;

	  data->imgOffset = Point(i*blockStride.width, j*blockStride.height);
	}
    }
}

void MyHog::myGammaCorrection(Mat& img, float fgamma)
{
  CV_Assert(img.data);

  //accept only uchar type
  CV_Assert(img.depth() != sizeof(uchar));

  //build up look-up table
  uchar lut[256];
  for (int i = 0; i < 256; i++)
    {
      lut[i] = saturate_cast<uchar>(pow((float)(i / 255.0f), fgamma)*255.0f);
    }

  MatIterator_<uchar> it, end;
  for (it = img.begin<uchar>(), end = img.end<uchar>(); it != end; it++)
    {
      *it = lut[(*it)];
    }
}

void MyHog::computeGradient(const Mat& imgSrc)
{
  Mat imgGray;
  cvtColor(imgSrc, imgGray, CV_BGR2GRAY);

  //build up look-up table
  float fgamma = 0.5;
  float lut[256];
  for (int i = 0; i < 256; i++)
    {
      lut[i] = pow((float)i, fgamma);
    }
	
  Mag[0] = Mat::zeros(imgGray.size(), CV_32FC1);
  Mag[1] = Mat::zeros(imgGray.size(), CV_32FC1);
  AngleOf[0] = Mat::zeros(imgGray.size(), CV_8UC1);
  AngleOf[1] = Mat::zeros(imgGray.size(), CV_8UC1);
  Dx = Mat::zeros(imgGray.size(), CV_32FC1);
  Dy = Mat::zeros(imgGray.size(), CV_32FC1);
	
  //calculate the gradient and its direction
  int Rows = imgGray.rows;
  int Cols = imgGray.cols;
  for (int i = 1; i < Rows-1; i++)
    {
      for (int j = 1; j < Cols-1; j++)
	{
	  //with the gamma correction
	  float dx = (float)(lut[imgGray.at<uchar>(i, j + 1)] - lut[imgGray.at<uchar>(i, j - 1)]);
	  //cout << "dx: " << dx << endl;
	  float dy = (float)(lut[imgGray.at<uchar>(i + 1, j)] - lut[imgGray.at<uchar>(i - 1, j)]);
	  //cout << "dy: " << dy << endl;

	  Dx.at<float>(i, j) = dx;
	  Dy.at<float>(i, j) = dy;
	
	  float mag = dx*dx + dy*dy;
	  //cout << "mag: " << mag << endl;
	  float angle = (float)atan2((double)dy , (double)dx);
			
	  nBins = 9;
	  float angleScale = (float)(nBins / CV_PI);
	  float bin = angle*angleScale;
	  int hidx = cvFloor(bin);
	  bin -= hidx;
			
	  Mag[0].at<float>(i, j) = mag * (1.f - bin);
	  Mag[1].at<float>(i, j) = mag * bin;

	  if (hidx < 0)
	    hidx += nBins;
	  else if (hidx >= nBins)
	    {
	      hidx -= nBins;
	    }

	  AngleOf[0].at<uchar>(i, j) = (uchar)hidx;
	  hidx++;
	  if (hidx >= nBins)
	    {
	      hidx = 0;
	    }
	  AngleOf[1].at<uchar>(i, j) = (uchar)hidx;
			
			
	}
    }

  //imshow("Mag[0]", Mag[0]);
  //imshow("Mag[1]", Mag[1]);

  //imshow("Angle", Angle);
  //imshow("Dx", Dx);
  //imshow("Dy", Dy);

  //HOGDescriptor hog;
  //Mat grad;
  //Mat angleOf;

  //hog = HOGDescriptor(Size(64, 128), Size(16, 16), Size(8, 8), Size(8, 8), 9);
  //hog.computeGradient(imgGray, grad, angleOf);
  //Mat grad2[2], angleOf2[2];

  //split(grad, grad2);
  //imshow("grad_x", grad2[0]);
  //imshow("grad_y", grad2[1]);
  //
  //split(angleOf, angleOf2);

  //build the histogram
}

const float* MyHog::getHistOfBlock(Point imgOffset, float* dbuf)
{
  Point pt = imgOffset + mWinOffset;

  float* blockHist = dbuf;

  int k, C1 = count1, C2 = count2, C4 = count4;

  const PixelData* data = &pixelData[0];
  for (k = 0; k < C1; k++)
    {
      const PixelData& px = data[k];

      float grad0 = *Mag[0].ptr<float>(px.gradOfs);
      float grad1 = *Mag[1].ptr<float>(px.gradOfs);
        
      uchar angle0 = *AngleOf[0].ptr<uchar>(px.qangleOfs);
      uchar angle1 = *AngleOf[1].ptr<uchar>(px.qangleOfs);

      float weight = px.histWeightOfs[0] * px.gradWeight;
      float* ptrHist = blockHist + px.histOfs[0];
      float t0 = *(ptrHist + angle0) + grad0*weight;
      float t1 = *(ptrHist + angle1) + grad1*weight;
	        
      *(ptrHist + angle0) = t0;
      *(ptrHist + angle1) = t1;
    }
  for (; k < C1 + C2; k++)
    {
      const PixelData& px = data[k];

      float grad0 = *Mag[0].ptr<float>(px.gradOfs);
      float grad1 = *Mag[1].ptr<float>(px.gradOfs);

      uchar angle0 = *AngleOf[0].ptr<uchar>(px.qangleOfs);
      uchar angle1 = *AngleOf[1].ptr<uchar>(px.qangleOfs);

      float* ptrHist = blockHist + px.histOfs[0];
      float weight0 = px.histWeightOfs[0] * px.gradWeight;
      float t0 = *(ptrHist + angle0) + grad0*weight0;
      float t1 = *(ptrHist + angle1) + grad1*weight0;

      *(ptrHist + angle0) = t0;
      *(ptrHist + angle1) = t1;

      ptrHist = blockHist + px.histOfs[1];
      float weight1 = px.histWeightOfs[1] * px.gradWeight;
      t0 = *(ptrHist + angle0) + grad0*weight1;
      t1 = *(ptrHist + angle1) + grad1*weight1;

      *(ptrHist + angle0) = t0;
      *(ptrHist + angle1) = t1;
    }
  for (; k < C1 + C2 + C4; k++)
    {
      const PixelData& px = data[k];

      float grad0 = *Mag[0].ptr<float>(px.gradOfs);
      float grad1 = *Mag[1].ptr<float>(px.gradOfs);

      uchar angle0 = *AngleOf[0].ptr<uchar>(px.qangleOfs);
      uchar angle1 = *AngleOf[1].ptr<uchar>(px.qangleOfs);

      float* ptrHist = blockHist + px.histOfs[0];
      float weight0 = px.gradWeight*px.histWeightOfs[0];
      float t0 = *(ptrHist + angle0) + grad0*weight0;
      float t1 = *(ptrHist + angle1) + grad1*weight0;

      *(ptrHist + angle0) = t0;
      *(ptrHist + angle1) = t1;

      ptrHist = blockHist + px.histOfs[1];
      float weight1 = px.gradWeight*px.histWeightOfs[1];
      t0 = *(ptrHist + angle0) + grad0*weight1;
      t1 = *(ptrHist + angle1) + grad1*weight1;

      *(ptrHist + angle0) = t0;
      *(ptrHist + angle1) = t1;

      ptrHist = blockHist + px.histOfs[2];
      float weight2 = px.gradWeight*px.histWeightOfs[2];
      t0 = *(ptrHist + angle0) + grad0*weight2;
      t1 = *(ptrHist + angle1) + grad1*weight2;

      *(ptrHist + angle0) = t0;
      *(ptrHist + angle1) = t1;

      ptrHist = blockHist + px.histOfs[3];
      float weight3 = px.gradWeight*px.histWeightOfs[3];
      t0 = *(ptrHist + angle0) + grad0*weight3;
      t1 = *(ptrHist + angle1) + grad1*weight3;

      *(ptrHist + angle0) = t0;
      *(ptrHist + angle1) = t1;
    }

  normalizeBlockHistogram(blockHist);

  return blockHist;
}

void MyHog::compute(const Mat& imgSrc, vector<float>descriptor){
  size_t dsize = nBins*nCells.area()*nBlocks.area();
}

void MyHog::normalizeBlockHistogram(float* _hist) {
  float* hist = &_hist[0];
#ifdef HAVE_IPP
  size_t sz = blockHistogramSize;
#else
  size_t i, sz = mBlockHistogramSize;
#endif
		
  float sum = 0;
#ifdef HAVE_IPP
  ippsDotProd_32f(hist, hist, sz, &sum);
#else
  for (i = 0; i < sz; i++)
    sum += hist[i] * hist[i];
#endif
  float scale = 1.f / (std::sqrt(sum) + sz*0.1f);
#ifdef HAVE_IPP
  ippsMulC_32f_I(scale, hist, sz);
  ippsThreshold_32f_I(hist, sz, thresh, ippCmpGreater);
  ippsDotProd_32f(hist, hist, sz, &sum);
#else
  for (i = 0, sum = 0; i < sz; i++)
    {
      hist[i] = hist[i] * scale;
      sum += hist[i] * hist[i];
    }
#endif
		
  scale = 1.f / (std::sqrt(sum) + 1e-3f);
#ifdef HAVE_IPP
  ippsMulC_32f_I(scale, hist, sz);
#else
  for (i = 0; i < sz; i++)
    hist[i] *= scale;
#endif
}
