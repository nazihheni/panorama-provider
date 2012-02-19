#include "stdafx.h"
#include <math.h>
#include "RansacHomography.h"

#define SQRT2 (1.4142135623730951f)
RansacHomography::RansacHomography(double threshold,double probability)
{
	m_storage=cvCreateMemStorage(NULL);
	m_threshold=threshold;
	m_probability=probability;
}

void RansacHomography::NormailizePoints(const CvMat& matSrcPts, CvMat* matDstPts)
{
	//Normalize points
	int n = matSrcPts.rows;
	float xmean = 0, ymean = 0;
	for (int i = 0; i < n; i++)
	{
		xmean += CV_MAT_ELEM(matSrcPts, float, i, 0);
		ymean += CV_MAT_ELEM(matSrcPts, float, i, 1);
	}

	xmean /= n; ymean /= n;

	float scale = 0, x, y;
	for (int i = 0; i < n; i++)
	{
		x = CV_MAT_ELEM(matSrcPts, float, i, 0) - xmean;
		y = CV_MAT_ELEM(matSrcPts, float, i, 1) - ymean;

		scale += sqrt(x * x + y * y);
	}

	scale = (float)(SQRT2 * n / scale);

	for (int i = 0; i < n; i++)
	{
		x = CV_MAT_ELEM(matSrcPts, float, i, 0) - xmean;
		y = CV_MAT_ELEM(matSrcPts, float, i, 1) - ymean;

		scale += sqrt(x * x + y * y);
	}
	
	//transform the points
	float proj[]={
                    scale, 0.0f, -scale * xmean,
                    0.0f, scale, -scale * ymean,
                    0.0f, 0.0f, 1.0f
	};

	CvMat matProj=cvMat(3,3,CV_32F,proj);
	float xTrans, yTrans, w;
	for (int i = 0; i < n; i++)
	{
		//看看这个能否向量计算
		x = CV_MAT_ELEM(matSrcPts, float, i, 0);
		y = CV_MAT_ELEM(matSrcPts, float, i, 1);

		w = CV_MAT_ELEM(matSrcPts, float, 2, 0) * x
			+ CV_MAT_ELEM(matSrcPts, float, 2, 1) * y + 1.0f;

		xTrans = (CV_MAT_ELEM(matSrcPts, float, 0, 0) * x 
			+ CV_MAT_ELEM(matSrcPts, float, 0, 1) * y + CV_MAT_ELEM(matSrcPts, float, 0, 2)) / w;
        yTrans = (CV_MAT_ELEM(matSrcPts, float, 0, 3) * x 
			+ CV_MAT_ELEM(matSrcPts, float, 1, 0) * y + CV_MAT_ELEM(matSrcPts, float, 1, 1)) / w;

		CV_MAT_ELEM(*matDstPts, float, i, 0) = xTrans;
		CV_MAT_ELEM(*matDstPts, float, i, 1) = yTrans;
	}

}

void RansacHomography::Compute(CvMat& matSrcPts)
{

}

int RansacHomography::RunRansac(CvMat& matSrcPts,CvMat& matDstPts)
{
	if(matSrcPts.rows != matDstPts.rows) // The number of points should be equal.
	{
		return -1;
	}
	if(matSrcPts.rows < 4)//At least four points are required to fit an homography
	{
		return -2;
	}

	CvMat* matSrcTrans=cvCreateMat(matSrcPts.rows,matSrcPts.cols,matSrcPts.type);
	CvMat* matDstTrans=cvCreateMat(matDstPts.rows,matDstPts.cols,matDstPts.type);
	NormailizePoints(matSrcPts,matSrcTrans);
	NormailizePoints(matDstPts,matDstTrans);




	cvReleaseMat(&matSrcTrans);
	cvReleaseMat(&matDstTrans);
	return 0;
}