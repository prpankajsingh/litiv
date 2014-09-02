#include "BackgroundSubtractorPBAS_1ch.h"
#include "DistanceUtils.h"
#include "RandUtils.h"
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iomanip>

BackgroundSubtractorPBAS_1ch::BackgroundSubtractorPBAS_1ch(	 size_t nInitColorDistThreshold
															,float fInitUpdateRate
															,size_t nBGSamples
															,size_t nRequiredBGSamples)
	:	BackgroundSubtractorPBAS(nInitColorDistThreshold,fInitUpdateRate,nBGSamples,nRequiredBGSamples) {}

BackgroundSubtractorPBAS_1ch::~BackgroundSubtractorPBAS_1ch() {}

void BackgroundSubtractorPBAS_1ch::initialize(const cv::Mat& oInitImg) {
	CV_Assert(!oInitImg.empty() && oInitImg.cols>0 && oInitImg.rows>0);
	CV_Assert(oInitImg.type()==CV_8UC1);
	m_oImgSize = oInitImg.size();
	m_oDistThresholdFrame.create(m_oImgSize,CV_32FC1);
	m_oDistThresholdFrame = cv::Scalar(1.0f);
#if BGSPBAS_USE_R2_ACCELERATION
	m_oDistThresholdVariationFrame.create(m_oImgSize,CV_32FC1);
	m_oDistThresholdVariationFrame = cv::Scalar(BGSPBAS_R2_LOWER);
#endif //BGSPBAS_USE_R2_ACCELERATION
	m_oUpdateRateFrame.create(m_oImgSize,CV_32FC1);
	m_oUpdateRateFrame = cv::Scalar(m_fDefaultUpdateRate);
	m_oMeanMinDistFrame.create(m_oImgSize,CV_32FC1);
	m_oMeanMinDistFrame = cv::Scalar(0.0f);
	m_oLastFGMask.create(m_oImgSize,CV_8UC1);
	m_oLastFGMask = cv::Scalar(0);
	m_oFloodedFGMask.create(m_oImgSize,CV_8UC1);
	m_oFloodedFGMask = cv::Scalar(0);
	CV_Assert(m_voBGImg.size()==(size_t)m_nBGSamples);
	CV_Assert(m_voBGGrad.size()==(size_t)m_nBGSamples);
	cv::Mat oBlurredInitImg;
	cv::GaussianBlur(oInitImg,oBlurredInitImg,cv::Size(3,3),0,0,cv::BORDER_DEFAULT);
	cv::Mat oBlurredInitImg_GradX, oBlurredInitImg_GradY;
	cv::Scharr(oBlurredInitImg,oBlurredInitImg_GradX,CV_16S,1,0,1,0,cv::BORDER_DEFAULT);
	cv::Scharr(oBlurredInitImg,oBlurredInitImg_GradY,CV_16S,0,1,1,0,cv::BORDER_DEFAULT);
	cv::Mat oBlurredInitImg_AbsGradX, oBlurredInitImg_AbsGradY;
	cv::convertScaleAbs(oBlurredInitImg_GradX,oBlurredInitImg_AbsGradX);
	cv::convertScaleAbs(oBlurredInitImg_GradY,oBlurredInitImg_AbsGradY);
	cv::Mat oBlurredInitImg_AbsGrad;
	cv::addWeighted(oBlurredInitImg_AbsGradX,0.5,oBlurredInitImg_AbsGradY,0.5,0,oBlurredInitImg_AbsGrad);
	for(size_t s=0; s<m_nBGSamples; s++) {
		m_voBGImg[s].create(m_oImgSize,CV_8UC1);
		m_voBGImg[s] = cv::Scalar(0);
		m_voBGGrad[s].create(m_oImgSize,CV_8UC1);
		m_voBGGrad[s] = cv::Scalar(0);
		for(int y=0; y<m_oImgSize.height; ++y) {
			for(int x=0; x<m_oImgSize.width; ++x) {
				int x_sample,y_sample;
				getRandSamplePosition(x_sample,y_sample,x,y,0,m_oImgSize);
				m_voBGImg[s].at<uchar>(y,x) = oInitImg.at<uchar>(y_sample,x_sample);
				m_voBGGrad[s].at<uchar>(y,x) = oBlurredInitImg_AbsGrad.at<uchar>(y_sample,x_sample);
			}
		}
	}
	m_bInitialized = true;
}

void BackgroundSubtractorPBAS_1ch::operator()(cv::InputArray _image, cv::OutputArray _fgmask, double learningRateOverride) {
	CV_Assert(m_bInitialized);
	cv::Mat oInputImg = _image.getMat();
	CV_Assert(oInputImg.type()==CV_8UC1 && oInputImg.size()==m_oImgSize);
	_fgmask.create(m_oImgSize,CV_8UC1);
	cv::Mat oFGMask = _fgmask.getMat();
	oFGMask = cv::Scalar_<uchar>(0);
	cv::Mat oBlurredInputImg;
	cv::GaussianBlur(oInputImg,oBlurredInputImg,cv::Size(3,3),0,0,cv::BORDER_DEFAULT);
	cv::Mat oBlurredInputImg_GradX, oBlurredInputImg_GradY;
	cv::Scharr(oBlurredInputImg,oBlurredInputImg_GradX,CV_16S,1,0,1,0,cv::BORDER_DEFAULT);
	cv::Scharr(oBlurredInputImg,oBlurredInputImg_GradY,CV_16S,0,1,1,0,cv::BORDER_DEFAULT);
	cv::Mat oBlurredInputImg_AbsGradX, oBlurredInputImg_AbsGradY;
	cv::convertScaleAbs(oBlurredInputImg_GradX,oBlurredInputImg_AbsGradX);
	cv::convertScaleAbs(oBlurredInputImg_GradY,oBlurredInputImg_AbsGradY);
	cv::Mat oBlurredInputImg_AbsGrad;
	cv::addWeighted(oBlurredInputImg_AbsGradX,0.5,oBlurredInputImg_AbsGradY,0.5,0,oBlurredInputImg_AbsGrad);
	//cv::imshow("input grad mag",oBlurredInputImg_AbsGrad);
	//cv::Mat oSampleColorAbsDiff,oSampleGradAbsDiff;
	//cv::absdiff(oInputImg,m_voBGImg[0],oSampleColorAbsDiff);
	//cv::absdiff(oBlurredInputImg_AbsGrad,m_voBGGrad[0],oSampleGradAbsDiff);
	//cv::imshow("oSampleColorAbsDiff",oSampleColorAbsDiff);
	//cv::imshow("oSampleGradAbsDiff",oSampleGradAbsDiff);
	//cv::Mat oNewDistImg;
	//cv::addWeighted(oSampleGradAbsDiff,GRAD_WEIGHT_ALPHA/m_fFormerMeanGradDist,oSampleColorAbsDiff,1.0,0,oNewDistImg);
	//cv::imshow("oNewDistImg",oNewDistImg);
	//cv::Mat oNewDistImgDiff;
	//cv::absdiff(oNewDistImg,oSampleColorAbsDiff,oNewDistImgDiff);
	//cv::imshow("diff oNewDistImg",oNewDistImgDiff);
	size_t nFrameTotGradDist=0;
	size_t nFrameTotBadSamplesCount=1;
	static const size_t nChannelSize = UCHAR_MAX;
	for(int y=0; y<m_oImgSize.height; ++y) {
		for(int x=0; x<m_oImgSize.width; ++x) {
			const size_t idx_uchar = oInputImg.step.p[0]*y + x;
			const size_t idx_flt32 = idx_uchar*4;
			float fMinDist=(float)nChannelSize;
			float* pfCurrDistThresholdFactor = (float*)(m_oDistThresholdFrame.data+idx_flt32);
			const float fCurrDistThreshold = ((*pfCurrDistThresholdFactor)*m_nDefaultColorDistThreshold);
			size_t nGoodSamplesCount=0, nSampleIdx=0;
			while(nGoodSamplesCount<m_nRequiredBGSamples && nSampleIdx<m_nBGSamples) {
				const size_t nColorDist = absdiff_uchar(oInputImg.data[idx_uchar],m_voBGImg[nSampleIdx].data[idx_uchar]);
				const size_t nGradDist = absdiff_uchar(oBlurredInputImg_AbsGrad.data[idx_uchar],m_voBGGrad[nSampleIdx].data[idx_uchar]);
				const float fSumDist = std::min(((BGSPBAS_GRAD_WEIGHT_ALPHA/m_fFormerMeanGradDist)*nGradDist)+nColorDist,(float)nChannelSize);
				if(fSumDist<=fCurrDistThreshold) {
					if(fMinDist>fSumDist)
						fMinDist = fSumDist;
					nGoodSamplesCount++;
				}
				else {
					nFrameTotGradDist += nGradDist;
					nFrameTotBadSamplesCount++;
				}
				nSampleIdx++;
			}
			float* pfCurrMeanMinDist = ((float*)(m_oMeanMinDistFrame.data+idx_flt32));
			*pfCurrMeanMinDist = ((*pfCurrMeanMinDist)*(BGSPBAS_N_SAMPLES_FOR_MEAN-1) + (fMinDist/nChannelSize))/BGSPBAS_N_SAMPLES_FOR_MEAN;
			float* pfCurrLearningRate = ((float*)(m_oUpdateRateFrame.data+idx_flt32));
			if(nGoodSamplesCount<m_nRequiredBGSamples) {
				oFGMask.data[idx_uchar] = UCHAR_MAX;
				*pfCurrLearningRate += BGSPBAS_T_INCR/((*pfCurrMeanMinDist)*BGSPBAS_T_SCALE+BGSPBAS_T_OFFST);
				if((*pfCurrLearningRate)>BGSPBAS_T_UPPER)
					*pfCurrLearningRate = BGSPBAS_T_UPPER;
			}
			else {
				const size_t nLearningRate = learningRateOverride>0?(size_t)ceil(learningRateOverride):(size_t)ceil((*pfCurrLearningRate));
				if((rand()%nLearningRate)==0) {
					const size_t s_rand = rand()%m_nBGSamples;
					m_voBGImg[s_rand].data[idx_uchar] = oInputImg.data[idx_uchar];
					m_voBGGrad[s_rand].data[idx_uchar] = oBlurredInputImg_AbsGrad.data[idx_uchar];
				}
				if((rand()%nLearningRate)==0) {
					int x_rand,y_rand;
					getRandNeighborPosition_3x3(x_rand,y_rand,x,y,0,m_oImgSize);
					const size_t s_rand = rand()%m_nBGSamples;
#if BGSPBAS_USE_SELF_DIFFUSION
					m_voBGImg[s_rand].at<uchar>(y_rand,x_rand) = oInputImg.at<uchar>(y_rand,x_rand);
					m_voBGGrad[s_rand].at<uchar>(y_rand,x_rand) = oBlurredInputImg_AbsGrad.at<uchar>(y_rand,x_rand);
#else //!BGSPBAS_USE_SELF_DIFFUSION
					m_voBGImg[s_rand].at<uchar>(y_rand,x_rand) = oInputImg.data[idx_uchar];
					m_voBGGrad[s_rand].at<uchar>(y_rand,x_rand) = oBlurredInputImg_AbsGrad.data[idx_uchar];
#endif //!BGSPBAS_USE_SELF_DIFFUSION
				}
				*pfCurrLearningRate -= BGSPBAS_T_DECR/((*pfCurrMeanMinDist)*BGSPBAS_T_SCALE+BGSPBAS_T_OFFST);
				if((*pfCurrLearningRate)<BGSPBAS_T_LOWER)
					*pfCurrLearningRate = BGSPBAS_T_LOWER;
			}
#if BGSPBAS_USE_R2_ACCELERATION
			float* pfCurrDistThresholdVariationFactor = (float*)(m_oDistThresholdVariationFrame.data+idx_flt32);
			if((*pfCurrMeanMinDist)>BGSPBAS_R2_OFFST && (oFGMask.data[idx_uchar]!=m_oLastFGMask.data[idx_uchar])) {
				if((*pfCurrDistThresholdVariationFactor)<BGSPBAS_R2_UPPER)
					(*pfCurrDistThresholdVariationFactor) += BGSPBAS_R2_INCR;
			}
			else {
				if((*pfCurrDistThresholdVariationFactor)>BGSPBAS_R2_LOWER)
					(*pfCurrDistThresholdVariationFactor) -= BGSPBAS_R2_DECR;
			}
			if((*pfCurrDistThresholdFactor)<BGSPBAS_R_LOWER+(*pfCurrMeanMinDist)*BGSPBAS_R_SCALE+BGSPBAS_R_OFFST) {
				if((*pfCurrDistThresholdFactor)<BGSPBAS_R_UPPER)
					(*pfCurrDistThresholdFactor) *= BGSPBAS_R_INCR*(*pfCurrDistThresholdVariationFactor);
			}
			else if((*pfCurrDistThresholdFactor)>BGSPBAS_R_LOWER)
				(*pfCurrDistThresholdFactor) *= BGSPBAS_R_DECR*(*pfCurrDistThresholdVariationFactor);
#else //!BGSPBAS_USE_R2_ACCELERATION
			if((*pfCurrDistThresholdFactor)<BGSPBAS_R_LOWER+(*pfCurrMeanMinDist)*BGSPBAS_R_SCALE+BGSPBAS_R_OFFST) {
				if((*pfCurrDistThresholdFactor)<BGSPBAS_R_UPPER)
					(*pfCurrDistThresholdFactor) *= BGSPBAS_R_INCR;
			}
			else if((*pfCurrDistThresholdFactor)>BGSPBAS_R_LOWER)
				(*pfCurrDistThresholdFactor) *= BGSPBAS_R_DECR;
#endif //!BGSPBAS_USE_R2_ACCELERATION
		}
	}
	m_fFormerMeanGradDist = std::max(((float)nFrameTotGradDist)/nFrameTotBadSamplesCount,20.0f);
	/*if(curr_debug_id==0) {
		cv::Point dbg1(60,40), dbg2(218,132);
		cv::Mat oMeanMinDistFrameNormalized = m_oMeanMinDistFrame;
		cv::circle(oMeanMinDistFrameNormalized,dbg1,5,cv::Scalar(1.0f));cv::circle(oMeanMinDistFrameNormalized,dbg2,5,cv::Scalar(1.0f));
		cv::imshow("m(x)",oMeanMinDistFrameNormalized);
		std::cout << std::fixed << std::setprecision(5) << " m(" << dbg1 << ") = " << m_oMeanMinDistFrame.at<float>(dbg1) << "  ,  m(" << dbg2 << ") = " << m_oMeanMinDistFrame.at<float>(dbg2) << std::endl;
		cv::Mat oDistThresholdFrameNormalized = (m_oDistThresholdFrame-cv::Scalar(R_LOWER))/(R_UPPER-R_LOWER);
		cv::circle(oDistThresholdFrameNormalized,dbg1,5,cv::Scalar(1.0f));cv::circle(oDistThresholdFrameNormalized,dbg2,5,cv::Scalar(1.0f));
		cv::imshow("r(x)",oDistThresholdFrameNormalized);
		std::cout << std::fixed << std::setprecision(5) << " r(" << dbg1 << ") = " << m_oDistThresholdFrame.at<float>(dbg1) << "  ,  r(" << dbg2 << ") = " << m_oDistThresholdFrame.at<float>(dbg2) << std::endl;
#if BGSPBAS_USE_R2_ACCELERATION
		cv::Mat oDistThresholdVariationFrameNormalized = (m_oDistThresholdVariationFrame-cv::Scalar(R2_LOWER))/(R2_UPPER-R2_LOWER);
		cv::circle(oDistThresholdVariationFrameNormalized,dbg1,5,cv::Scalar(1.0f));cv::circle(oDistThresholdVariationFrameNormalized,dbg2,5,cv::Scalar(1.0f));
		cv::imshow("r2(x)",oDistThresholdVariationFrameNormalized);
		std::cout << std::fixed << std::setprecision(5) << "r2(" << dbg1 << ") = " << m_oDistThresholdVariationFrame.at<float>(dbg1) << "  , r2(" << dbg2 << ") = " << m_oDistThresholdVariationFrame.at<float>(dbg2) << std::endl;
#endif //BGSPBAS_USE_R2_ACCELERATION
		cv::Mat oUpdateRateFrameNormalized = (m_oUpdateRateFrame-cv::Scalar(T_LOWER))/(T_UPPER-T_LOWER);
		cv::circle(oUpdateRateFrameNormalized,dbg1,5,cv::Scalar(1.0f));cv::circle(oUpdateRateFrameNormalized,dbg2,5,cv::Scalar(1.0f));
		cv::imshow("t(x)",oUpdateRateFrameNormalized);
		std::cout << std::fixed << std::setprecision(5) << " t(" << dbg1 << ") = " << m_oUpdateRateFrame.at<float>(dbg1) << "  ,  t(" << dbg2 << ") = " << m_oUpdateRateFrame.at<float>(dbg2) << std::endl;
		cv::waitKey(1);
	}*/
#if BGSPBAS_USE_ADVANCED_MORPH_OPS || BGSPBAS_USE_R2_ACCELERATION
	oFGMask.copyTo(m_oLastFGMask);
#endif //BGSPBAS_USE_ADVANCED_MORPH_OPS || BGSPBAS_USE_R2_ACCELERATION
#if BGSPBAS_USE_ADVANCED_MORPH_OPS
	//cv::imshow("pure seg",oFGMask);
	cv::medianBlur(oFGMask,oFGMask,3);
	//cv::imshow("median3",oFGMask);
	oFGMask.copyTo(m_oFloodedFGMask);
	cv::dilate(m_oFloodedFGMask,m_oFloodedFGMask,cv::Mat());
	//cv::imshow("median3 + dilate3",m_oFloodedFGMask);
	cv::erode(m_oFloodedFGMask,m_oFloodedFGMask,cv::Mat());
	//cv::imshow("median3 + dilate3 + erode3",m_oFloodedFGMask);
	cv::floodFill(m_oFloodedFGMask,cv::Point(0,0),255);
	cv::bitwise_not(m_oFloodedFGMask,m_oFloodedFGMask);
	//cv::imshow("median3 de3 fill region",m_oFloodedFGMask);
	cv::bitwise_or(m_oFloodedFGMask,m_oLastFGMask,oFGMask);
	//cv::imshow("median3 post-fill",oFGMask);
	cv::medianBlur(oFGMask,oFGMask,9);
	//cv::imshow("median3 post-fill, +median9 ",oFGMask);
	//cv::waitKey(0);
#else //!BGSPBAS_USE_ADVANCED_MORPH_OPS
	cv::medianBlur(oFGMask,oFGMask,9);
#endif //!BGSPBAS_USE_ADVANCED_MORPH_OPS
}
