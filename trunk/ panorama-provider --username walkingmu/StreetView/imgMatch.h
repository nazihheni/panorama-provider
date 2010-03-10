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
����ͼƬ���кܶ�ؼ��㣬����Ҳ������ƥ��
���Ա��ṹ���ڼ�����Щ�ؼ���Ĵ�����ʲô

������ͼ����ת������
����Ҫ�ĸ���������Ŵ�������ת�Ƕ�
*/
typedef struct IMGPOINT
{
	int Count; //ƥ��ĵ��Ĵ���
	CvPoint SmpPt1; //��ͼ�Ĺؼ���
	CvPoint SmpPt2;//��ͼ�Ĺؼ���
	CvPoint AltPt1;//��ͼ�Ĵιؼ���
	CvPoint AltPt2;//��ͼ�Ĵιؼ���
}imgPoint;
#define MAX_DISTANCE 5001 //����������X��Y������಻����5001������

typedef struct threadParams
{
	HWND hParentWnd;//�����������ΪNULL���򽫽��Ȼ㱨���ô���
	std::list<CString> lstImgs; //�ܶ�ͼƬ��·��
	BOOL bcmbForce; //��ʹ���������Ҳǿ�ƺϲ�
	LPVOID lpOut;//�����������ʱδ��
	IplImage* IplImageCombined;//�ϲ���ɵ�ͼƬ
}ThreadParams;

/* the maximum number of keypoint NN candidates to check during BBF search */
#define KDTREE_BBF_MAX_NN_CHKS 200

/* threshold on squared ratio of distances between NN and 2nd NN */
#define NN_SQ_DIST_RATIO_THR 0.49

DWORD WINAPI ImgHandleThread(LPVOID lpParam);