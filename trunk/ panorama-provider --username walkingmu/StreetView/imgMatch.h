#include "stdafx.h"

#include "sift.h"
#include "imgfeatures.h"
#include "kdtree.h"
#include "utils.h"
#include "xform.h"

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <windows.h>
#include <list>


#define WM_PROGRESS WM_USER+100
/*
两张图片会有很多关键点，其中也会有误匹配
所以本结构用于计算这些关键点的大方向是什么

若两张图有旋转或缩放
则需要四个点来计算放大倍数和旋转角度
*/
typedef struct IMGPOINT
{
	int Count; //匹配的到的次数
	CvPoint SmpPt1; //左图的关键点
	CvPoint SmpPt2;//右图的关键点
	CvPoint AltPt1;//左图的次关键点
	CvPoint AltPt2;//右图的次关键点
}imgPoint;
#define MAX_DISTANCE 5001 //左右两个点X或Y坐标最多不超过5001个像素

typedef struct threadParams
{
	HWND hParentWnd;//若这个参数不为NULL，则将进度汇报给该窗体
	std::list<CString> lstImgs; //很多图片的路径
	BOOL bcmbForce; //即使特征点很少也强制合并
	LPVOID lpOut;//输出参数，暂时未用
	IplImage* IplImageCombined;//合并完成的图片
}ThreadParams;

/* the maximum number of keypoint NN candidates to check during BBF search */
#define KDTREE_BBF_MAX_NN_CHKS 200

/* threshold on squared ratio of distances between NN and 2nd NN */
#define NN_SQ_DIST_RATIO_THR 0.49

DWORD WINAPI ImgHandleThread(LPVOID lpParam);