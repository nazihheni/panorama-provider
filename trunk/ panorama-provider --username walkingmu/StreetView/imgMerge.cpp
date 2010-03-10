/*
Function for merge serval images to a panorama

For more information refer to 
	Matthew Brown and David G. Lowe-
		Automatic Panoramic Image Stitching using Invariant Features

Copyright (C) 2010  Samson William Mulder <walkingmu@gmail.com>

version 20100228
*/

#include "sift.h"
#include "xform.h"
#include "imgMerge.h"


/*
Get how many SIFT features are matched between two pictures

@param img1 - the first image
@param img2 - the second image
*/
int GetMatchPointNumber(list<PanoImage>::iterator img1,
						list<PanoImage>::iterator img2)
{
	int i,k,num=0;
	double d0, d1;
	struct feature* feat1, * feat2, * feat;
	struct feature** nbrs;

	feat1=img1->featRev.feat;
	feat2=img2->featRev.feat;

	for( i = 0; i < img1->featRev.num; i++ )
	{
		feat = feat1 + i;
		//find the feature
		k = kdtree_bbf_knn( img2->featRev.kd_root, feat, 2, &nbrs, KDTREE_BBF_MAX_NN_CHKS );
		if( k == 2 )
		{
			d0 = descr_dist_sq( feat, nbrs[0] );
			d1 = descr_dist_sq( feat, nbrs[1] );
			if( d0 < d1 * NN_SQ_DIST_RATIO_THR )
			{
				num++; //count++
				feat1[i].fwd_match = nbrs[0];
			}
		}
		free( nbrs );
	}
	return num;
}

int MatchVerify()
{
	return MATCH_ACCEPT;
}

/*
After sift features generated, the fuction find potential images matches
for the current image. See section 3 of Brown's paper.

@param pitorImg - the current image
@param listImg - All the images
@param releation - output 
		select CANDIDATE_IMAGE_NUM images from all images which
		most simliar to current image
@return value: the number of candidates( no more than CANDIDATE_IMAGE_NUM)
*/
int GetCandidateImg(list<PanoImage>::iterator pitorImg,
							list<PanoImage>* lstImgs)
{
	int i,count=0,num=0;

	for(list<PanoImage>::iterator pitor; pitor!=lstImgs->end();pitor++)
	{
		/*
		if(pitor!=pitorImg) // No need to compare to itself
		{
			num=GetMatchPointNumber(pitor,pitorImg);
			//Add to the candidates
			for(i=0;i<CANDIDATE_IMAGE_NUM;i++)
			{
				if(relation->candidates[i].num<num)
				{
					count++;
					if(relation->candidates[i].feat!=NULL)
					{
						free(relation->candidates[i].feat);
						relation->candidates[i].feat=NULL;
					}
					relation->candidates[i].num=num;
					relation->candidates[i].panoImg=&(*pitor);
					//此处要看看到底要开多少空间，合并好之后到底是复制哪个
					relation->candidates[i].feat=calloc(pitor->featRev.num,sizeof(struct feature));
					memcpy(relation->relation->candidates[i].feat,
						pitor->featRev.feat,
						pitor->featRev.num*sizeof(struct feature));
					break;
				}
			}
		}*/
	}
	return count;
}

//Merge images to panorama
//parameters:

//return value: 0 for success, none zero for an error code
int StartMerge(MergeData imgData)
{
	/*
Note:
	memory recycle point:
		MergeData->lstpanoImgs->featRev->featurereciver->kd_root
		MergeData->lstpanoImgs->featRev->featurereciver->feat
	*/
	//Initial stage

	//stage 1: feature matching
	    //step1: generate SIFT features
	for(list<PanoImage>::iterator pImg=imgData.lstpanoImgs.begin();
		pImg!=imgData.lstpanoImgs.end(); pImg++){
			//generating the SIFT features
			pImg->featRev.num=sift_features(pImg->lstImg,&pImg->featRev.feat);
			pImg->featRev.kd_root=kdtree_build( pImg->featRev.feat, pImg->featRev.num);
	}

	//stage 2: image matching
	struct feature inliers;
	struct feature* pfeatInliers=&inliers;
	struct feature** ppfeatInliers=&(pfeatInliers);
	int numInliers=0;
	ransac_xform_fn xform_fn=lsq_homog;
	ransac_err_fn err_fn=homog_xfer_err;
	//initial step: find potential image matches
	for(list<PanoImage>::iterator pImg=imgData.lstpanoImgs.begin();
		pImg!=imgData.lstpanoImgs.end(); pImg++){
			//step 1: Robust Homography Estimation using RANSAC
			//compute RANSAC for each image
			CvMat* ransacMat=ransac_xform(pImg->featRev.feat,
				pImg->featRev.num,FEATURE_FWD_MATCH,xform_fn,
				MINI_NUM_OF_CORRESPONDCES,DESIRED_PROBABILTY,
				err_fn,CORRESPOINDENCES_OF_INLIER_DISTANCE
				,&ppfeatInliers,&numInliers);
	}
		
		//step 2:Probabilistic Model for Image Match Verification

	//stage 3: Bundle Adjustment
		//step 1: Fast Solution by Direct Computation of the Linear System
	
	//stage 4: Automatic Panorama Straightening
	//stage 5: Gain Compensation
	//stage 6: Multi-Band Blending

	return SUCCESS_NO_ERROR;
}