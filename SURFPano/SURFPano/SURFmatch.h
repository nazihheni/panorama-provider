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
	CvSeq* m_sqPoints; //������
	CvSeq* m_sqDesc;//����������
public:
	CvSurfFeatures();
	~CvSurfFeatures();

	void ExtractFeatures(IplImage* image, CvSURFParams& params);

	int CompareFeatures(CvSurfFeatures& surfFeatures, CvSURFCmpParam& param);
	int test();
};