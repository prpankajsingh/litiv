#pragma once

#include <opencv2/features2d/features2d.hpp>
#include <opencv2/video/background_segm.hpp>

//! defines the internal threshold adjustment factor to use when determining if the variation of a single channel is enough to declare the pixel as foreground
#define BGSPBAS_USE_SELF_DIFFUSION 1
//! defines whether to use or not the R2 acceleration thresholds to modulate R(x) variations
#define BGSPBAS_USE_R2_ACCELERATION 0
//! defines whether to use or not the advanced morphological operations
#define BGSPBAS_USE_ADVANCED_MORPH_OPS 0

//! defines the default value for BackgroundSubtractorPBAS::m_nDefaultColorDistThreshold
#define BGSPBAS_DEFAULT_COLOR_DIST_THRESHOLD (30)
//! defines the default value for BackgroundSubtractorPBAS::m_nBGSamples
#define BGSPBAS_DEFAULT_NB_BG_SAMPLES (35)
//! defines the default value for BackgroundSubtractorPBAS::m_nRequiredBGSamples
#define BGSPBAS_DEFAULT_REQUIRED_NB_BG_SAMPLES (2)
//! defines the default value for BackgroundSubtractorPBAS::m_fDefaultUpdateRate
#define BGSPBAS_DEFAULT_LEARNING_RATE (16.0f)
//! defines the default value for the learning rate passed to BackgroundSubtractorViBeLBSP::operator()
#define BGSPBAS_DEFAULT_LEARNING_RATE_OVERRIDE (-1.0)
//! parameters used for dynamic threshold adjustments
#define BGSPBAS_R_OFFST (0.0000f)
#define BGSPBAS_R_SCALE (5.0000f)
#define BGSPBAS_R_INCR  (1.0500f)
#define BGSPBAS_R_DECR  (0.9500f)
#define BGSPBAS_R_LOWER (0.6000f)
#define BGSPBAS_R_UPPER (99.0000f)
//! parameters used for adjusting the variation speed of dynamic thresholds
#if BGSPBAS_USE_R2_ACCELERATION
#define BGSPBAS_R2_OFFST (0.075f)
#define BGSPBAS_R2_INCR  (0.005f)
#define BGSPBAS_R2_DECR  (0.001f)
#define BGSPBAS_R2_LOWER (0.950f)
#define BGSPBAS_R2_UPPER (1.050f)
#endif //BGSPBAS_USE_R2_ACCELERATION
//! parameters used for dynamic learning rate adjustments
#define BGSPBAS_T_OFFST (1.0000f)
#define BGSPBAS_T_SCALE (255.00f)
#define BGSPBAS_T_DECR  (0.0500f)
#define BGSPBAS_T_INCR  (1.0000f)
#define BGSPBAS_T_LOWER (2.0000f)
#define BGSPBAS_T_UPPER (200.00f)
//! weight ratio attributed to gradient intensities when mixing with color
#define BGSPBAS_GRAD_WEIGHT_ALPHA (10.0f)
//! number of samples used to create running averages for model variation computations
#define BGSPBAS_N_SAMPLES_FOR_MEAN (m_nBGSamples)

/*!
	PBAS foreground-background segmentation algorithm (abstract version).

	For more details on the different parameters, go to @@@@@@@@@@@@@@.

	This algorithm is currently NOT thread-safe.
 */
class BackgroundSubtractorPBAS : public cv::BackgroundSubtractor {
public:
	//! full constructor
	BackgroundSubtractorPBAS(	int nInitColorDistThreshold=BGSPBAS_DEFAULT_COLOR_DIST_THRESHOLD,
								float fInitUpdateRate=BGSPBAS_DEFAULT_LEARNING_RATE,
								int nBGSamples=BGSPBAS_DEFAULT_NB_BG_SAMPLES,
								int nRequiredBGSamples=BGSPBAS_DEFAULT_REQUIRED_NB_BG_SAMPLES);
	//! default destructor
	virtual ~BackgroundSubtractorPBAS();
	//! (re)initiaization method; needs to be called before starting background subtraction
	virtual void initialize(const cv::Mat& oInitImg)=0;
	//! primary model update function; the learning param is used to override the internal learning speed (ignored when <= 0)
	virtual void operator()(cv::InputArray image, cv::OutputArray fgmask, double learningRateOverride=BGSPBAS_DEFAULT_LEARNING_RATE_OVERRIDE)=0;
	//! @@@@@@@@@@@@ ????
	virtual cv::AlgorithmInfo* info() const;
	//! returns a copy of the latest reconstructed background image
	void getBackgroundImage(cv::OutputArray backgroundImage) const;

protected:
	//! number of different samples per pixel/block to be taken from input frames to build the background model ('N' in the original ViBe/PBAS papers)
	const int m_nBGSamples;
	//! number of similar samples needed to consider the current pixel/block as 'background' ('#_min' in the original ViBe/PBAS papers)
	const int m_nRequiredBGSamples;
	//! background model pixel intensity samples
	std::vector<cv::Mat> m_voBGImg;
	//! background model pixel gradient samples
	std::vector<cv::Mat> m_voBGGrad;
	//! input image size
	cv::Size m_oImgSize;
	//! absolute color distance threshold ('R' or 'radius' in the original ViBe paper, and the default 'R(x)' value in the original PBAS paper)
	const int m_nDefaultColorDistThreshold;
	//! per-pixel distance thresholds ('R(x)' in the original PBAS paper)
	cv::Mat m_oDistThresholdFrame;
	//! per-pixel distance thresholds variation
	cv::Mat m_oDistThresholdVariationFrame;
	//! per-pixel mean minimal decision distances ('D(x)' in the original PBAS paper)
	cv::Mat m_oMeanMinDistFrame;
	//! the last foreground mask returned by the method (used for blinking pixel detection)
	cv::Mat m_oLastFGMask;
	//! the 'flooded' foreground mask, using for filling holes in blobs
	cv::Mat m_oFloodedFGMask;
	//! absolute default update rate threshold (the default 'T(x)' value in the original PBAS paper)
	const float m_fDefaultUpdateRate;
	//! mean gradient magnitude distance over the past frame
	float m_fFormerMeanGradDist;
	//! per-pixel update rate ('T(x)' in the original PBAS paper)
	cv::Mat m_oUpdateRateFrame;
	//! defines whether or not the subtractor is fully initialized
	bool m_bInitialized;
};