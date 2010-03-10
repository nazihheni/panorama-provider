#include "stdafx.h"
#include "imgMatch.h"

#define OUTPUT_MATCH_POINT_AS_FILE 1

static TCHAR msg[540];
static HWND hWnd;
static WORD Progress;
void SendNote()
{
	if(hWnd !=NULL && IsWindow(hWnd))
	{
		SendMessage(hWnd,WM_PROGRESS,(WPARAM)msg,Progress);
	}
}

//��ȡ����֮�����
double GetDistance(CvPoint cvPoint1,CvPoint cvPoint2)
{
	//ʹ�ù�ʽdist=sqrt[ (x2-x1)^2 + (y2-y1)^2]
	double tmp1=cvPoint2.x-cvPoint1.x;
	double tmp2=cvPoint2.y-cvPoint2.y;
	return sqrt(tmp1*tmp1+tmp2*tmp2);
}

//��ȡ����֮��ļн�
//�н�Ϊ�������������X��ļн�
double GetAngle(CvPoint cvPoint1,CvPoint cvPoint2)
{
	//���߳���
	double tmpA=abs(cvPoint2.x-cvPoint1.x); //X�����ֱ�Ǳ�
	double tmpB=abs(cvPoint2.y-cvPoint1.y); //Y�����ֱ�Ǳ�
	double tmpC=sqrt(tmpA*tmpA+tmpB*tmpB); //����

	if(tmpC==0.0)
		return tmpC;
	//AngleB�Ǳ�B����Ӧ�Ľǣ���tmpA��tmpC�ļнǣ������ƣ�
	double AngleB=asin(tmpB/tmpC);
	return AngleB;
}

//˳ʱ����ת
//angle�ǻ����Ƶģ���math.h�ṩ��acos,cos����һ��
void reRotate(IplImage *src,double angle,IplImage *dst)
{
  /* the first command line parameter must be image file name */
      //int delta = 1;
      //int angle = 45; //�Ƕ���
      int opt = 0;		// 1�� ��ת������
      // 0:  ������ת
      double factor;
 
      dst = cvCloneImage (src);
      //cvNamedWindow ("src", 1);
      //cvShowImage ("src", src);
 
	  //do{
	  float m[6];
	  // Matrix m looks like:
	  //
	  // [ m0  m1  m2 ] ===>  [ A11  A12   b1 ]
	  // [ m3  m4  m5 ]       [ A21  A22   b2 ]
	  //dst(x+width(dst)/2, y+height(dst)/2)= src( A11x+A12y+b1, A21x+A22y+b2),

	  CvMat M = cvMat (2, 3, CV_32F, m);
	  int w = src->width;
	  int h = src->height;
	  if (opt)		// ��ת������
	    factor = (cos (angle) + 1.0) * 2;
	  else			//  ������ת
	    factor = 1;
	  m[0] = (float) (factor * 2 * cos (-angle));
	  m[1] = (float) (factor * 2* sin (-angle));
	  m[3] = -m[1];
	  m[4] = m[0];
	  // ����ת��������ͼ���м�
	  m[2] = w * 0.5f;
	  m[5] = h * 0.5f;
	  //  dst(x,y) = A * src(x,y) + b
	  cvZero (dst);
	  cvGetQuadrangleSubPix (src, dst, &M);

	  cvSaveImage("rotated.jpg",src,0);
	  //cvNamedWindow ("dst", 1);
	  //cvShowImage ("dst", dst);

	  //angle =(angle + delta) % 360;
	//}while(cvWaitKey(0)!=27);// for-loop
}

void AdjustRoation(imgPoint* SmpPoint1,imgPoint* SmpPoint2,IplImage* src)
{
	double dist1=GetDistance(SmpPoint1->SmpPt1,SmpPoint1->SmpPt2);
	double dist2=GetDistance(SmpPoint2->SmpPt1,SmpPoint2->SmpPt2);
	
	double Angle1=GetAngle(SmpPoint1->SmpPt1,SmpPoint1->SmpPt2);
	double Angle2=GetAngle(SmpPoint2->SmpPt1,SmpPoint2->SmpPt2);

	double Rotation=Angle2-Angle1; //��ת�Ƕ�
	double Magnitude=dist1/dist2;  //��С�任

	_stprintf_s(msg,540,_T("Angle1:%f, Angle2:%f \r\nRotate images for %f"),
		Angle1,Angle2,Rotation);
	SendNote();
	//�����Ƕ�
	IplImage* dst=cvCloneImage(src);
	reRotate(dst,Rotation,src);
	
	cvReleaseImage(&dst);

	//˳������תN��
	//x=x*cos(N)+y*sin(N)
	//y=-x*sin(N)+ycos(N)
	SmpPoint1->SmpPt2.x=static_cast<int>(SmpPoint1->SmpPt2.x * cos(Rotation)
		+ SmpPoint1->SmpPt2.y * sin(Rotation));
	SmpPoint1->SmpPt2.y=static_cast<int>(SmpPoint1->SmpPt2.y * cos(Rotation)
		- SmpPoint1->SmpPt2.x * sin(Rotation));
}


void Combine(IplImage* img1, IplImage* img2,IplImage** stacked,IplImage** imgMount, HWND hWnd)
{
	struct feature* feat1, * feat2, * feat;
	struct feature** nbrs;
	struct kd_node* kd_root;
	CvPoint pt1, pt2;
	double d0, d1;
	int n1, n2, k, i, m = 0;
	static imgPoint ipOrgPt[MAX_DISTANCE]; //���ڼ���ԭʼ��

	memset(ipOrgPt,0,sizeof(imgPoint)*MAX_DISTANCE);

	*stacked = stack_imgs( img1, img2 ); //��img2���ӵ�img1���棬��ŵ�img1
	if( !img1 || !img2)
	{
		return ;
	}

	_stprintf_s(msg,540,_T("Calculating sift features for the first image..."));
	SendNote();
	n1 = sift_features( img1, &feat1 );

	_stprintf_s(msg,540,_T("Calculating sift features for the second image..."));
	SendNote();
	n2 = sift_features( img2, &feat2 );

	_stprintf_s(msg,540,_T("Building kdtree..."));
	SendNote();
	kd_root = kdtree_build( feat2, n2 );

	_stprintf_s(msg,540,_T("Finding relevant points..."));
	SendNote();

#ifdef OUTPUT_MATCH_POINT_AS_FILE
	CFile fileMatchPt(_T("D:\\point.txt"),CFile::modeNoTruncate|CFile::modeCreate|CFile::modeWrite);
	int szBuff=0;
#endif
	for( i = 0; i < n1; i++ )
	{
		feat = feat1 + i;
		k = kdtree_bbf_knn( kd_root, feat, 2, &nbrs, KDTREE_BBF_MAX_NN_CHKS );
		if( k == 2 )
		{
			d0 = descr_dist_sq( feat, nbrs[0] );
			d1 = descr_dist_sq( feat, nbrs[1] );
			if( d0 < d1 * NN_SQ_DIST_RATIO_THR )
			{
				pt1 = cvPoint( cvRound( feat->x ), cvRound( feat->y ) );
				pt2 = cvPoint( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );
				
#ifdef OUTPUT_MATCH_POINT_AS_FILE
				szBuff=_stprintf_s(msg,540,
					_T("(%d,%d) match to (%d,%d), difference(%d,%d)\r\n")
					,pt1.x,pt1.y,pt2.x,pt2.y,pt1.x-pt2.x,pt1.y-pt2.y);
				fileMatchPt.Write(msg,szBuff * sizeof(TCHAR));
#endif
				int hash=abs(pt1.x-pt2.x);
				if(hash>MAX_DISTANCE)
					hash=MAX_DISTANCE-1;
				ipOrgPt[hash].Count++;
				if(ipOrgPt[hash].Count==1)
				{
					ipOrgPt[hash].SmpPt1=pt1;
					ipOrgPt[hash].SmpPt2=pt2;
				}else if(ipOrgPt[hash].Count==2){
					ipOrgPt[hash].AltPt1=pt1;
					ipOrgPt[hash].AltPt2=pt2;
				}
				
				pt2.y += img1->height; //��Ϊimg2�ϲ���img1�����ˣ����Լ���height������stack���λ��
				cvLine( *stacked, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );
				m++;
				feat1[i].fwd_match = nbrs[0];
			}
		}
		free( nbrs );
	}

#ifdef OUTPUT_MATCH_POINT_AS_FILE
	fileMatchPt.Flush();
	fileMatchPt.Close();
#endif

	//��ȡ�����
	int MaxCount=0;
	imgPoint* ipMax=&ipOrgPt[0]; //�غϴ������ĵ�
	imgPoint* ipNext=NULL;//�غϴ����ζ�ĵ�
	for(i=0;i<MAX_DISTANCE;i++)
	{
		if(ipOrgPt[i].Count>MaxCount)
		{
			MaxCount=ipOrgPt[i].Count;
			ipNext=ipMax; //ԭ�����������Ǵζ��
			ipMax=&ipOrgPt[i];//��������
		}
	}
		
	_stprintf_s(msg,540,_T("Rotating images..."));
	SendNote();
	AdjustRoation(ipMax,ipNext,img2);

	_stprintf_s(msg,540,_T("combining images..."));
	SendNote();
	int x1=0,y1=0,x2=0,y2=0;
	//����ϳɺ��ͼƬ��С
	int dstWidth=0,dstHeight=0;
	//������
	dstWidth+=(ipMax->SmpPt1.x > ipMax->SmpPt2.x ?
		ipMax->SmpPt1.x : ipMax->SmpPt2.x);
	dstWidth+=(img1->width-ipMax->SmpPt1.x >  img2->width-ipMax->SmpPt2.x ? 
		img1->width-ipMax->SmpPt1.x : img2->width-ipMax->SmpPt2.x);
	//����߶�
	dstHeight+=(ipMax->SmpPt1.y > ipMax->SmpPt2.y ?
		ipMax->SmpPt1.y : ipMax->SmpPt2.y);
	dstHeight+=(img1->height-ipMax->SmpPt1.y >  img2->height-ipMax->SmpPt2.y ? 
		img1->height-ipMax->SmpPt1.y : img2->height-ipMax->SmpPt2.y);

//��������ͼƬӦ�÷���ʲô�ط�
	if(ipMax->SmpPt1.x<ipMax->SmpPt2.x
		&& ipMax->SmpPt1.y<ipMax->SmpPt2.y)
	{
		x1=ipMax->SmpPt2.x-ipMax->SmpPt1.x;
		y1=ipMax->SmpPt2.y-ipMax->SmpPt1.y;
		x2=0;
		y2=0;
	}else if(ipMax->SmpPt1.x>ipMax->SmpPt2.x
		&& ipMax->SmpPt1.y<ipMax->SmpPt2.y)
	{
		x1=0;
		y1=ipMax->SmpPt2.y-ipMax->SmpPt1.y;
		x2=ipMax->SmpPt1.x-ipMax->SmpPt2.x;
		y2=0;
	}else if(ipMax->SmpPt1.x<=ipMax->SmpPt2.x
		&& ipMax->SmpPt1.y>=ipMax->SmpPt2.y)
	{
		x1=ipMax->SmpPt2.x-ipMax->SmpPt1.x;
		y1=0;
		x2=0;
		y2=ipMax->SmpPt1.y-ipMax->SmpPt2.y;
	}
	else{
		x1=0;
		y1=0;
		x2=ipMax->SmpPt1.x-ipMax->SmpPt2.x;
		y2=ipMax->SmpPt1.y-ipMax->SmpPt2.y;
	}

	CvSize cvsize;
	cvsize.height=dstHeight;
	cvsize.width=dstWidth;
	
	*imgMount=cvCreateImage(cvsize,img1->depth,img1->nChannels);

	CvRect cvrect1,cvrect2;
	cvrect1.x=x1;
	cvrect1.y=y1;
	cvrect1.width=img1->width;
	cvrect1.height=img1->height;

	cvSetImageROI(*imgMount,cvrect1);
	cvrect2.x=0;
	cvrect2.y=0;
	cvrect2.width=img1->width;
	cvrect2.height=img1->height;
	cvSetImageROI(img1,cvrect2);
	try{
		cvCopy(img1,*imgMount,NULL);
	}catch(cv::Exception ex){
		OutputDebugStringA(&ex.err[0]);
	}
	cvResetImageROI(img1);
	cvResetImageROI(*imgMount);

	cvrect1.x=x2;
	cvrect1.y=y2;
	cvrect1.width=img2->width;
	cvrect1.height=img2->height;

	cvSetImageROI(*imgMount,cvrect1);
	cvrect2.x=0;
	cvrect2.y=0;
	cvrect2.width=img2->width;
	cvrect2.height=img2->height;
	cvSetImageROI(img2,cvrect2);
	try{
		cvCopy(img2,*imgMount,NULL);
	}catch(cv::Exception ex){
		OutputDebugStringA(&ex.err[0]);
	}
	cvResetImageROI(img2);
	cvResetImageROI(*imgMount);
	
	kdtree_release( kd_root );
	free( feat1 );
	free( feat2 );
}

DWORD WINAPI ImgHandleThread(LPVOID lpParam)
{
	static char cSavePath[MAX_PATH]={NULL};
	ThreadParams* pParam=reinterpret_cast<ThreadParams*>(lpParam);
	hWnd=pParam->hParentWnd;
	Progress=0;
	std::list<CString>::iterator pitor=pParam->lstImgs.begin();

	static IplImage* img1, *img2,*stacked,*imgMount;
	char ImgPath1[MAX_PATH];
	
	stacked=NULL;
	WideCharToMultiByte(CP_UTF8,CP_ACP,
		pitor->GetBuffer(),-1,ImgPath1,MAX_PATH,NULL,NULL);

	_stprintf_s(msg,540,_T("initializing %s"),pitor->GetBuffer());
	SendNote();
	img1=cvLoadImage(ImgPath1,1);

	int i=0;
	if(++pitor!=pParam->lstImgs.end())
	{
		do{
			WideCharToMultiByte(CP_UTF8,CP_ACP,
				pitor->GetBuffer(),-1,ImgPath1,MAX_PATH,NULL,NULL);

			_stprintf_s(msg,540,_T("initializing %s"),pitor->GetBuffer());
			SendNote();

			img2=cvLoadImage(ImgPath1,1);
			Combine(img1,img2,&stacked,&imgMount,pParam->hParentWnd);
			pitor++;

			i++;
			sprintf_s(cSavePath,MAX_PATH,"tmp%d.jpg",i);
			cvSaveImage(cSavePath,stacked,0);
			_stprintf_s(msg,540,_T("Temporary match file has been saved to tmp%d.jpg in current process's folder.")
				,i);
			SendNote();
			//����ж���ļ�������ļ��ϲ��������ϲ���һ���ļ�
			if(pitor->GetLength()==0 || pitor==pParam->lstImgs.end())
			{
				break;
			}
			
			cvReleaseImage( &stacked );
			cvReleaseImage( &img2 );
			cvReleaseImage(&img1);
			img1=imgMount;
		}while(true);

		_stprintf_s(msg,540,_T("The result has been saved to result.jpg in current process's folder."));
		pParam->IplImageCombined=imgMount;
		cvSaveImage("Result.jpg",imgMount,0);
		SendNote();
		//������Դ
		cvReleaseImage( &stacked );
		cvReleaseImage( &img2 );
	}
	cvReleaseImage( &img1 );

	Progress=100;
	_stprintf_s(msg,540,_T("Finished."));
	SendNote();
	return 0;
}