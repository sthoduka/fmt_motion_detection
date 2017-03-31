#ifndef MOTION_DETECTION_H
#define MOTION_DETECTION_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <imreg_fmt/image_registration.h>
#include <fmt_motion_detection/image_differencing.h>

class MotionDetection
{
    public:
        MotionDetection(const cv::Mat &im);
        virtual ~MotionDetection();

        /*
         * Detect motion between im and previous image (from previous call to this function)
         * 1. Register previous image to im
         * 2. Absolute difference + thresholding + edge mask
         *
         * returns a set of bounding boxes
         */
        std::vector<cv::Rect> detectMotion(const cv::Mat &im, cv::Mat &output, bool display_images = false);

        /**
         * Set threshold value
         * Pixels above this value will be set to 255
         */
        void setDifferenceIntensityThreshold(int intensity_threshold) {difference_intensity_threshold_ = intensity_threshold;}

        /**
         * Whether or not to use an edge mask
         */
        void setUseEdgeMask(bool use_edge_mask) {use_edge_mask_ = use_edge_mask;}

    protected:
        ImageRegistration image_registration_;
        ImageDifferencing image_differencing_;

        int difference_intensity_threshold_;
        // edges with area lower than this are masked
        double difference_area_threshold_;
        // points closer than this threshold are considered to be part of the same cluster
        double cluster_euclidean_threshold_;

        cv::Mat im0_registered_;
        cv::Mat edge_mask_;
        cv::Mat previous_image_;

        bool use_edge_mask_;
};

#endif /* MOTION_DETECTION_H */
