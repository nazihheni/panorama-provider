#include <cv.h>
#include <highgui.h>

#ifndef IN
	#define IN
#endif
#ifndef OUT
	#define OUT
#endif

typedef struct _cvSURFCmpParam_
{
	IN double ratio;
	OUT CvSeq* sqMatchPtSrc;
	OUT CvSeq* sqMatchPtDst;
}CvSURFCmpParam, *pCvSURFCmpParam;

class CvSurfFeatures
{
private:
	CvMemStorage* m_storage;
	CvSeq* m_sqPoints; //特征点
	CvSeq* m_sqDesc;//特征描述子
public:
	CvSurfFeatures();
	~CvSurfFeatures();

	void ExtractFeatures(IplImage* image, CvSURFParams& params);

	int CompareFeatures(CvSurfFeatures& surfFeatures, CvSURFCmpParam& param);
	int test();
};