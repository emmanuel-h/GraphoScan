#include "HOG.h"

using namespace cv;
using namespace std;

void MyHog::init(Mat& imgSrc, Size winSize, Size blockSize, Size blockStride, Size cellSize)
{

	//weights为一个尺寸为blockSize的二维高斯表,下面的代码就是计算二维高斯的系数
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

	//一个block中的像素个数
	int rawBlockSize = mBlockSize.width*mBlockSize.height;
	
	//按一个window中的block个数重新分配长度
	blockData.resize(nBlocks.width*nBlocks.height);

	//按一个block中的像素个数重新分配长度
	pixelData.resize(rawBlockSize * 3);
	count1 = count2 = count4 = 0;

	//根据习惯,从X坐标开始再到Y坐标, 即从列到行
	for (int i = 0; i < mBlockSize.width; i++)
		for (int j = 0; j < mBlockSize.height; j++)
		{
			PixelData* data = 0;

			//cellX和cellY表示的是block内该像素点所在的cell横坐标和纵坐标索引，以小数的形式存在
			//为了判断该像素对哪个cell有贡献,在默认参数下(blockSize(16,16),cellSize(8,8))此处将其划分为-1, 0, 1三个区间(- 0.5f), 分别对应一个,二个,四个
			//i+ 0.5f是为了使坐标奇数？
			float cellX = (i + 0.5f) / mCellSize.width - 0.5f;
			float cellY = (j + 0.5f) / mCellSize.height - 0.5f;

			int iCellX0 = cvFloor(cellX);
			int iCellY0 = cvFloor(cellY);
			int iCellX1 = iCellX0 + 1, iCellY1 = iCellY0 + 1;

			//cellX和cellY现在为到所属的cell的
			cellX -= iCellX0;
			cellY -= iCellY0;

			if (iCellX0 == 0) 
				//iCellX0等于0，X坐标处在中间
			{
				if (iCellY0 == 0)//Y坐标也处在中间，表示该像素处在中间，对四个Cells均有贡献
				{
					data = &pixelData[rawBlockSize * 2 + (count4++)];

					//对cell0的直方图的贡献
					data->histOfs[0] = (iCellX0 * nCells.height + iCellY0)*nBins;
					//其权重, 按到cell中心距离计算
					data->histWeightOfs[0] = (1.0f - cellX) * (1.0f - cellY);

					//对cell1
					data->histOfs[1] = (iCellX1 * nCells.height + iCellY0)*nBins;
					//其权重, 按到cell中心距离计算
					data->histWeightOfs[1] = cellX * (1.0 - cellY);

					//对cell2
					data->histOfs[2] = (iCellX0 * nCells.height + iCellY1)*nBins;
					//其权重, 按到cell中心距离计算
					data->histWeightOfs[2] = (1.0f - cellX) *  cellY;

					//对cell3
					data->histOfs[3] = (iCellX1 * nCells.height + iCellY1)*nBins;
					//其权重, 按到cell中心距离计算
					data->histWeightOfs[3] = cellX * cellY;
				}
				else//Y坐标处在上下两侧，因此表示只对两个Cells有贡献
				{
					data = &pixelData[rawBlockSize * 1 + (count2++)];
			
					if (iCellY0 == 1)
						//在这个默认参数中，每个block只有四个cells
						//当iCellY0 == -1时，不满足此条件
						//iCellY1 = iCellY0? 因为当iCellY0 == -1时，进行以下计算需要使用iCllY1 = iCellY0 + 1
					{
						iCellY1 = iCellY0;
						cellY = 1.0f - cellY;//到倒对称中心的距离，应1.0f - cellY
					}
					//对cell0
					data->histOfs[0] = (iCellX0 * nCells.height + iCellY1)*nBins;
					//其权重, 按到cell中心距离计算
					data->histWeightOfs[0] = (1.0f - cellX) * cellY;

					//对cell1
					data->histOfs[1] = (iCellX1 * nCells.height + iCellY1)*nBins;
					//其权重, 按到cell中心距离计算
					data->histWeightOfs[1] = cellX * cellY;

					//对cell2和cell3
					data->histOfs[2] = data->histOfs[3] = 0;
					//其权重, 按到cell中心距离计算
					data->histWeightOfs[2] = data->histWeightOfs[3] = 0;
				}
			}
			else //iCellX0等于-1或1,X坐标处在两边
			{	
				if (iCellX0 == 1)
				{
					iCellX1 = iCellX0;
					cellX = 1.0f - cellX;
				}

				if (iCellY0 == 0)
					//Y坐标在中间，对两个cells有贡献
				{
					data = &pixelData[rawBlockSize * 1 + (count2++)];

					//对上方cell
					data->histOfs[0] = (iCellX1 * nCells.height + iCellY0)*nBins;
					data->histWeightOfs[0] = cellX *  (1.0f - cellY);

					//对下方cell
					data->histOfs[1] = (iCellX1 * nCells.height + iCellY1) * nBins;
					data->histWeightOfs[1] = cellX * cellY;

					//对其它
					data->histOfs[2] = data->histOfs[3] = 0;
					data->histWeightOfs[2] = data->histWeightOfs[3] = 0;
				}
				else
					//Y坐标在两侧，只对一个cell有贡献
				{
					data = &pixelData[rawBlockSize * 0 + (count1++)];

					if (iCellY0 == 1)
					{
						iCellY1 = iCellY0;
						cellY = 1.0f - cellY;
					}

					//只对自身cell有贡献
					data->histOfs[0] = (iCellX1*nCells.height + iCellY1)*nBins;
					data->histWeightOfs[0] = cellX*cellY;

					//其它cells
					data->histOfs[1] = data->histOfs[2] = data->histOfs[3] = 0;
					data->histWeightOfs[1] = data->histWeightOfs[2] = data->histWeightOfs[3] = 0;

				}
			}

			//grad和angle在block中的坐标
			data->gradOfs = i*Mag[0].cols + j;
			data->qangleOfs = i*AngleOf[0].cols+ j;
			
			//权重，按高斯分配
			data->gradWeight = weights(i, j);
		}

		//保证每个点都遍历了
		assert(count1 + count2 + count4 == rawBlockSize);

		//将pixelData内存空间对齐
		for (int i = 0; i < count2; i++)
		{
			pixelData[i + count1] = pixelData[rawBlockSize + i];
		}
		for (int i = 0; i < count4; i++)
		{
			pixelData[i + count1 + count2] = pixelData[rawBlockSize * 2 + i];
		}

		//初始化blockData
		int blockHistogramSize = 36;	//nCells.width * nCells.height * nBins
		for (int i = 0; i < nBlocks.width; i++)
		{
			for (int j = 0; j < nBlocks.height; j++)
			{
				BlockData* data = &blockData[i*nBlocks.height + j];

				data->histOfs = (i*nBlocks.height + j)*blockHistogramSize;

				//block在window中的坐标
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

			//保存与该梯度方向相邻左右的两个bin编号
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
	//imgOffset为block在window中的坐标,mWinOffset是window在整个图片中的偏移坐标
	Point pt = imgOffset + mWinOffset;

	//dbuf为在block中的hist的偏移量
	float* blockHist = dbuf;

	int k, C1 = count1, C2 = count2, C4 = count4;

	const PixelData* data = &pixelData[0];
	for (k = 0; k < C1; k++)
	{
		const PixelData& px = data[k];

		//幅度指针及权重
		float grad0 = *Mag[0].ptr<float>(px.gradOfs);
		float grad1 = *Mag[1].ptr<float>(px.gradOfs);
	
		//相位指针
		uchar angle0 = *AngleOf[0].ptr<uchar>(px.qangleOfs);
		uchar angle1 = *AngleOf[1].ptr<uchar>(px.qangleOfs);

		//Hist指针
		//Cell0
		float weight = px.histWeightOfs[0] * px.gradWeight;
		float* ptrHist = blockHist + px.histOfs[0];
		float t0 = *(ptrHist + angle0) + grad0*weight;
		float t1 = *(ptrHist + angle1) + grad1*weight;
		
		//存放加权后的值
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

		//对Cell0的贡献
		float* ptrHist = blockHist + px.histOfs[0];
		float weight0 = px.histWeightOfs[0] * px.gradWeight;
		float t0 = *(ptrHist + angle0) + grad0*weight0;
		float t1 = *(ptrHist + angle1) + grad1*weight0;

		//存放加权后的值Cell0
		*(ptrHist + angle0) = t0;
		*(ptrHist + angle1) = t1;

		//对Cell1的贡献
		ptrHist = blockHist + px.histOfs[1];
		float weight1 = px.histWeightOfs[1] * px.gradWeight;
		t0 = *(ptrHist + angle0) + grad0*weight1;
		t1 = *(ptrHist + angle1) + grad1*weight1;

		//存放加权后的值Cell1
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

		//对Cell0
		float* ptrHist = blockHist + px.histOfs[0];
		float weight0 = px.gradWeight*px.histWeightOfs[0];
		float t0 = *(ptrHist + angle0) + grad0*weight0;
		float t1 = *(ptrHist + angle1) + grad1*weight0;

		//存放加权后的值Cell0
		*(ptrHist + angle0) = t0;
		*(ptrHist + angle1) = t1;

		//对Cell1
		ptrHist = blockHist + px.histOfs[1];
		float weight1 = px.gradWeight*px.histWeightOfs[1];
		t0 = *(ptrHist + angle0) + grad0*weight1;
		t1 = *(ptrHist + angle1) + grad1*weight1;

		//存放加权后的值Cell1
		*(ptrHist + angle0) = t0;
		*(ptrHist + angle1) = t1;

		//对Cell2
		ptrHist = blockHist + px.histOfs[2];
		float weight2 = px.gradWeight*px.histWeightOfs[2];
		t0 = *(ptrHist + angle0) + grad0*weight2;
		t1 = *(ptrHist + angle1) + grad1*weight2;

		//存放加权后的值Cell2
		*(ptrHist + angle0) = t0;
		*(ptrHist + angle1) = t1;

		//对Cell3
		ptrHist = blockHist + px.histOfs[3];
		float weight3 = px.gradWeight*px.histWeightOfs[3];
		t0 = *(ptrHist + angle0) + grad0*weight3;
		t1 = *(ptrHist + angle1) + grad1*weight3;

		//存放加权后的值Cell3
		*(ptrHist + angle0) = t0;
		*(ptrHist + angle1) = t1;
	}

	normalizeBlockHistogram(blockHist);

	return blockHist;
}

void MyHog::compute(const Mat& imgSrc, vector<float>descriptor)
{
	//一个window中的hog的长度
	size_t dsize = nBins*nCells.area()*nBlocks.area();



}

void MyHog::normalizeBlockHistogram(float* _hist) 
 {
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
		     //第一次归一化求的是平方和
		     for (i = 0; i < sz; i++)
		         sum += hist[i] * hist[i];
	 #endif
		     //分母为平方和开根号+0.1
			 float scale = 1.f / (std::sqrt(sum) + sz*0.1f);
	 #ifdef HAVE_IPP
		     ippsMulC_32f_I(scale, hist, sz);
	     ippsThreshold_32f_I(hist, sz, thresh, ippCmpGreater);
	     ippsDotProd_32f(hist, hist, sz, &sum);
	 #else
		     for (i = 0, sum = 0; i < sz; i++)
		     {
		         //第2次归一化是在第1次的基础上继续求平和和
			         hist[i] = hist[i] * scale;
		         sum += hist[i] * hist[i];
		     }
	 #endif
		
		     scale = 1.f / (std::sqrt(sum) + 1e-3f);
#ifdef HAVE_IPP
		     ippsMulC_32f_I(scale, hist, sz);
#else
		     //最终归一化结果
		     for (i = 0; i < sz; i++)
		        hist[i] *= scale;
#endif
	}
