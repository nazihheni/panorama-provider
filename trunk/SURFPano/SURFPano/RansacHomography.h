#include <cv.h>
#include <list>

using std::list;
typedef list<int> PtIdxList;
#ifndef IN
	#define IN
#endif
#ifndef OUT
	#define OUT
#endif

class RansacHomography
{
private:
	CvMemStorage* m_storage;
	CvSeq* m_sqInliners;
	CvSeq* m_sqOutliners;
	double m_threshold;
	double m_probability;

	void NormailizePoints(const CvMat& matSrcPts, CvMat* matDstPts);

	void Compute(CvMat& matSrcPts);
	/// <summary>
    ///   Estimates a homography with the given points.
    /// </summary>
    void homography(PtIdxList points, CvMat& H )
    {
        // Retrieve the original points
		

        // Compute the homography
        
     }

	/// <summary>
    ///   Compute inliers using the Symmetric Transfer Error,
    /// </summary>
    void distance(CvMat& H, double t, PtIdxList inliners)
    {
        // Compute the projections (both directions)

        // Compute the distances
        
            // Compute the distance as
     

        // Find and return the inliers
        
    }
	/// <summary>
    ///   Checks if the selected points will result in a degenerate homography.
    /// </summary>
    bool degenerate(PtIdxList points)
    {
        // If any three of the four points in each set is colinear,
        //  the resulting homography matrix will be degenerate.

        return true;
    }
public:
	/// <summary>
    ///   Creates a new RANSAC homography estimator.
    /// </summary>
    /// <param name="threshold">Inlier threshold.</param>
    /// <param name="probability">Inlier probability.</param>
	RansacHomography(double threshold,double probability);
	~RansacHomography();

	/// <summary>
    ///  Run ransac, the two input mat should contain at least 4 matched points.
	///  Each row of the mat should contain x,y
    /// </summary>
    /// <param name="sqMatchPtSrc">A mat contains a set of points.</param>
    /// <param name="sqMatchPtDst">A mat contains a set of points.</param>
	int RunRansac(CvMat& matSrcPts,CvMat& matDstPts);
};
