#include <fmt_motion_detection/motion_detection.h>
#include <sstream>
#include <iomanip>

MotionDetection::MotionDetection(const cv::Mat &im) :
    image_registration_(im),
    difference_intensity_threshold_(40), difference_area_threshold_((im.cols * im.rows)/2560.0),
    cluster_euclidean_threshold_((double)im.cols / 10.0),
    use_edge_mask_(true)
{
    edge_mask_ = cv::Mat(im.rows, im.cols, CV_8U, 255);
    im.copyTo(previous_image_);
}

MotionDetection::~MotionDetection()
{
}

std::vector<cv::Rect> MotionDetection::detectMotion(const cv::Mat &im, cv::Mat &output, bool display_images)
{
    cv::Mat difference;
    cv::Mat difference_no_edges;
    std::vector<double> transform_params(4);
    std::vector<std::vector<cv::Point> > clusters;
    int min_area = (im.cols * im.rows) / 1920;
    std::vector<cv::Rect> bounding_rectangles;
    std::vector<cv::Rect> filtered_bounding_rectangles;

    im.copyTo(output);
    im.copyTo(previous_image_);

    // 1. register image
    image_registration_.registerImage(im, im0_registered_, transform_params, display_images);

    // 2. temporal differencing
    image_differencing_.findDifference(image_registration_.getCurrentImage(), im0_registered_, difference, difference_intensity_threshold_, display_images);

    // 3. binary threshold
    image_differencing_.threshold(difference, difference_intensity_threshold_, difference, display_images);

    // 4. edge and border masking
    image_differencing_.getEdgeMask(difference, edge_mask_, difference_area_threshold_);
    image_differencing_.combineMasks(image_registration_.getBorderMask(), edge_mask_);
    if (use_edge_mask_)
    {
        image_differencing_.applyMask(difference, edge_mask_, difference_no_edges);
    }
    else
    {
        image_differencing_.applyMask(difference, image_registration_.getBorderMask(), difference_no_edges);
    }

    if (display_images)
    {
        cv::imshow("difference_masked", difference_no_edges);
    }

    // 5. find contours, and cluster contour points
    image_differencing_.clusterContourPartition(difference_no_edges, cluster_euclidean_threshold_, clusters, min_area);
    // 6. fit bounding boxes to clusters
    image_differencing_.getBoundingRectangles(clusters, bounding_rectangles);

    // 7. filter bounding boxes based on size and amount of motion (as seen in difference image)
    for (int i = 0; i < bounding_rectangles.size(); i++)
    {
        // find pixels which are in motion
        cv::Mat nonZeroPoints;
        cv::findNonZero(difference_no_edges(bounding_rectangles[i]), nonZeroPoints);

        double max = 0.0;
        double motion_ratio = (double)nonZeroPoints.total() / bounding_rectangles[i].area();
        double min_magnitude = im.cols / 160.0;
        double max_magnitude = im.cols / 10.0;
        double min_area = im.cols * 8;
        if (bounding_rectangles[i].area() < min_area)
        {
        }
        else if (motion_ratio < 0.1)
        {
        }
        else
        {
            filtered_bounding_rectangles.push_back(bounding_rectangles[i]);
        }
    }
    image_differencing_.drawBoundingRectangles(filtered_bounding_rectangles, output);
    cv::imshow("output", output);
    image_registration_.next();
    return filtered_bounding_rectangles;
}

