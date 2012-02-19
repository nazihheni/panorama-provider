#include "stdafx.h"
#include "SURFmatch.h"
#include <math.h>

CvSurfFeatures::CvSurfFeatures()
{
	m_storage = cvCreateMemStorage(NULL);
}


CvSurfFeatures::~CvSurfFeatures()
{
	cvReleaseMemStorage(&m_storage);
}

void CvSurfFeatures::ExtractFeatures(IplImage* image, CvSURFParams& params)
{
	cvExtractSURF(image, NULL, &m_sqPoints, &m_sqDesc, m_storage, params, NULL);
}

int CvSurfFeatures::CompareFeatures(CvSurfFeatures& surfFeatures, CvSURFCmpParam& param)
{
	CvSeq* img1KPoints,* img2KPoints;
	CvSeq* img1Descriptors,* img2Descriptors;
	CvSeq* sqMatchPtSrc, *sqMatchPtDst;

	img1KPoints = this->m_sqPoints;
	img2KPoints = surfFeatures.m_sqPoints;

	img1Descriptors=this->m_sqDesc;
	img2Descriptors=surfFeatures.m_sqDesc;


	sqMatchPtSrc = cvCreateSeq( CV_32FC2, sizeof(CvSeq), sizeof(CvPoint), m_storage );
	sqMatchPtDst = cvCreateSeq( CV_32FC2, sizeof(CvSeq), sizeof(CvPoint), m_storage );

	//two image's descriptor numbers.
	int img1DescNum = img1Descriptors->total;
	int img2DescNum = img2Descriptors->total;
	int length = img1Descriptors->elem_size / sizeof(float);//Descriptor length

	CvMat* desc1Mat,* desc2Mat;
	desc1Mat = cvCreateMat(img1DescNum, length, CV_32FC1);
	desc2Mat = cvCreateMat(img2DescNum, length, CV_32FC1);

	//copy descriptor sequence to matrix
	for (int i(0); i < img1DescNum; i++)
	{
	   float* curDesc = (float*)cvGetSeqElem(img1Descriptors, i);
	   for (int k(0); k < length; k++)
		  CV_MAT_ELEM(*desc1Mat, float, i, k) = curDesc[k];
	}
	for (int i(0); i < img2DescNum; i++)
	{
	   float* curDesc = (float*)cvGetSeqElem(img2Descriptors, i);
	   for (int k(0); k < length; k++)
		  CV_MAT_ELEM(*desc2Mat, float, i, k) = curDesc[k];
	}

	//Uses KD-Tree in OpenCV to match key points:
	CvFeatureTree* KDTree = cvCreateKDTree(desc1Mat);
	CvMat* matResults,* matDist;
	matResults = cvCreateMat(img2DescNum, 2, CV_32S);
	matDist = cvCreateMat(img2DescNum, 2, CV_64FC1);
	cvFindFeatures(KDTree, desc2Mat, matResults, matDist, 2, 400);//To find 2 nearest neighbors
	int numMatched(0);

	CvPoint2D32f pt1,pt2;
	for (int i(0); i < matDist->rows; i++)
	{
	   double dist1 = CV_MAT_ELEM(*matDist, double, i, 0);
	   double dist2 = CV_MAT_ELEM(*matDist, double, i, 1);
	   double tmp1 = MIN(dist1, dist2);
	   double tmp2 = MAX(dist1, dist2);
	   dist1 = tmp1; dist2 = tmp2;
	   if (dist1 < param.ratio * dist2)// 最近的两个距离比值小于0.6则认为此点匹配成功
	   {
		  int idx1, idx2;
		  idx1 = CV_MAT_ELEM(*matResults, int, i, 0);
		  idx2 = CV_MAT_ELEM(*matResults, int, i, 1);
		  CvSURFPoint* SURFpt1,* SURFpt2;
		  SURFpt1 = (CvSURFPoint*)cvGetSeqElem(img1KPoints, idx1);
		  SURFpt2 = (CvSURFPoint*)cvGetSeqElem(img2KPoints, idx2);
		  
		  pt1=(SURFpt1->pt);
		  pt2=(SURFpt2->pt);
		  
		  cvSeqPush(sqMatchPtSrc,&pt1);
		  cvSeqPush(sqMatchPtDst,&pt2);
		  numMatched++;
	   }
	}

	param.sqMatchPtSrc = sqMatchPtSrc;
	param.sqMatchPtDst = sqMatchPtDst;
	return numMatched;
}

int CvSurfFeatures::test()
{
	IplImage* image1=cvLoadImage("D:\\code\\opensource\\autostitch\\images\\test\\1.jpg",0);
	IplImage* image2=cvLoadImage("D:\\code\\opensource\\autostitch\\images\\test\\2.jpg",0);

	//stack the image
	IplImage* stacked = cvCreateImage( cvSize( MAX(image1->width, image2->width),
										image1->height + image2->height ),
										IPL_DEPTH_8U, image1->nChannels );

	cvZero( stacked );
	cvSetImageROI( stacked, cvRect( 0, 0, image1->width, image1->height ) );
	cvAdd( image1, stacked, stacked, NULL );
	cvSetImageROI( stacked, cvRect(0, image1->height, image2->width, image2->height) );
	cvAdd( image2, stacked, stacked, NULL );
	cvResetImageROI( stacked );

	int i;
	CvSURFParams params = cvSURFParams(500, 1);
	CvSURFCmpParam param = {0.6}; 
	CvSurfFeatures feat1;
	CvSurfFeatures feat2;
	
	feat1.ExtractFeatures(image1,params);
	feat2.ExtractFeatures(image2,params);

	feat1.CompareFeatures(feat2,param);

	CvSeq* sqMatchPtSrc=param.sqMatchPtSrc;
	CvSeq* sqMatchPtDst=param.sqMatchPtDst;

	if(sqMatchPtSrc->total != sqMatchPtDst->total)
	{
		return 1;
	}

	CvPoint pt1,pt2;
	CvPoint2D32f *fpt1,*fpt2;
	CvPoint ptEnd;
	for(i=0; i< sqMatchPtSrc->total; i++)
	{
		fpt1 = ((CvPoint2D32f*)cvGetSeqElem(sqMatchPtSrc, i));
		fpt2 = ((CvPoint2D32f*)cvGetSeqElem(sqMatchPtDst, i));

		pt1=cvPointFrom32f(*fpt1);
		pt2=cvPointFrom32f(*fpt2);

		ptEnd.x=pt2.x;
		ptEnd.y=image1->height + pt2.y;
		cvLine(stacked,pt1,ptEnd,CV_RGB(0,0,255));
	}
	//哥决定折腾自己的RANSAC了

	CvMat* mat1=cvCreateMat(sqMatchPtSrc->total,3,CV_32F);
	CvMat* mat2=cvCreateMat(sqMatchPtDst->total,3,CV_32F);

	CvMat* matNorm1=cvCreateMat(sqMatchPtSrc->total,3,CV_32F);
	CvMat* matNorm2=cvCreateMat(sqMatchPtDst->total,3,CV_32F);

	CvMat* H=cvCreateMat(3,3,CV_32F);
	CV_MAT_ELEM(*H, float, 2, 2) = 1.0f;
	try{
	for (int i(0); i < sqMatchPtSrc->total; i++)
	{
		fpt1 = (CvPoint2D32f*)cvGetSeqElem(sqMatchPtSrc, i);
		CV_MAT_ELEM(*mat1, float, i, 0) = fpt1->x;
		CV_MAT_ELEM(*mat1, float, i, 1) = fpt1->y;
		CV_MAT_ELEM(*mat1, float, i, 2) = 1.0f;
	}
	for (int i(0); i < sqMatchPtDst->total; i++)
	{
		fpt2 = (CvPoint2D32f*)cvGetSeqElem(sqMatchPtDst, i);
		CV_MAT_ELEM(*mat2, float, i, 0) = fpt2->x;
		CV_MAT_ELEM(*mat2, float, i, 1) = fpt2->y;
		CV_MAT_ELEM(*mat2, float, i, 2) = 1.0f;
	}
	}catch(cv::Exception ex)
	{
		OutputDebugStringA(&ex.err[0]);
	}

	cvConvertPointsHomogenious(mat1,matNorm1);
	cvConvertPointsHomogenious(mat2,matNorm2);

	cvFindHomography(matNorm1,matNorm2,H,CV_RANSAC,1.0);

	IplImage* imgTrans = cvCreateImage( cvSize( image1->width+ image2->width,
										MAX(image1->height , image2->height)),
										IPL_DEPTH_8U, image1->nChannels );

	cvWarpPerspective(image2,imgTrans,H);

	cvShowImage("Res",imgTrans);
	cvWaitKey(0);

	cvReleaseImage(&stacked);
	return 0;
}