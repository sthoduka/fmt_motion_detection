#ifndef IMAGE_DIFFERENCING_H
#define IMAGE_DIFFERENCING_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

class ImageDifferencing
{
    public:
        ImageDifferencing();
        virtual ~ImageDifferencing();
        /**
         * Absolute difference between im0 and im1
         */
        void findDifference(const cv::Mat &im0, const cv::Mat &im1, cv::Mat &diff, int threshold, bool display_images = false);

        /**
         * Binary threshold
         */
        void threshold(const cv::Mat &im, int threshold, cv::Mat &thresholded_image, bool display_images);

        /**
         * Returns edge mask
         */
        void getEdgeMask(const cv::Mat &im, cv::Mat &edge_mask, double area_threshold);

        /**
         * Applies a binary mask to in
         */
        void applyMask(const cv::Mat &in, const cv::Mat &mask, cv::Mat &out);

        /**
         * Adds mask1 to mask2 (bitwise OR)
         */
        void combineMasks(const cv::Mat &mask1, cv::Mat &mask2);

        /**
         * Returns which_percentile of pixel intensities in im
         */
        float percentile(const cv::Mat &im, double which_percentile);

        /**
         * Returns clusters of contour points from in
         */
        bool clusterContourPartition(const cv::Mat &in, double distance_threshold, std::vector<std::vector<cv::Point> > &clusters, int min_area);

        /**
         * Returns bounding boxes which have been fit to clusters
         */
        void getBoundingRectangles(const std::vector<std::vector<cv::Point> > &clusters, std::vector<cv::Rect> &bounding_rectangles);

        /**
         * Draws bounding boxes on out
         */
        void drawBoundingRectangles(const std::vector<cv::Rect> &bounding_rectangles, cv::Mat &out);
};

/**
 * predicate function for cv::partition
 * returns true if two points are closer to each other than threshold
 */
struct IsNeighbour
{
    double distance_threshold_squared;
    IsNeighbour(double distance_threshold)
    {
        distance_threshold_squared = distance_threshold * distance_threshold;
    }

    bool operator()(const cv::Point &p1, const cv::Point &p2) const
    {
        return (std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2)) < distance_threshold_squared;
    }
};

#endif /* IMAGE_DIFFERENCING_H */
