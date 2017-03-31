#include <fmt_motion_detection/image_differencing.h>
#include <iostream>

ImageDifferencing::ImageDifferencing()
{
}

ImageDifferencing::~ImageDifferencing()
{
}

void ImageDifferencing::findDifference(const cv::Mat &im0, const cv::Mat &im1, cv::Mat &diff, int threshold, bool display_images)
{
    float norm = percentile(im1, 0.90) / percentile(im0, 0.90);
    cv::Mat diff_first;
    cv::absdiff(im1, im0*norm, diff_first);

    diff_first.convertTo(diff_first, CV_32F, 255.0);
    diff_first.convertTo(diff_first, CV_8U);
    diff_first.copyTo(diff);

    if (display_images)
    {
        cv::imshow("abs_diff", diff);
    }
}

void ImageDifferencing::threshold(const cv::Mat &im, int threshold, cv::Mat &thresholded_image, bool display_images)
{
    cv::medianBlur(im, thresholded_image, 3);
    cv::inRange(thresholded_image, threshold, 255, thresholded_image);

    if (display_images)
    {
        cv::imshow("diff_threshold", thresholded_image);
    }

}

void ImageDifferencing::combineMasks(const cv::Mat &mask1, cv::Mat &mask2)
{
    cv::bitwise_and(mask1, mask2, mask2);
}

// assumes edge_mask is already the correct size
void ImageDifferencing::getEdgeMask(const cv::Mat &im, cv::Mat &edge_mask, double area_threshold)
{
    cv::Mat canny_output;
    double canny_threshold = 80.0;
    double canny_multiplier= 2.0;
    cv::Canny(im, canny_output, canny_threshold, canny_threshold * canny_multiplier, 3);
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(canny_output, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));
    std::vector<std::vector<cv::Point> > filtered_contours;
    std::vector<std::vector<cv::Point> > hulls;
    // reset edge mask
    edge_mask = cv::Scalar(255);
    int rect_width = im.rows / 45;
    int rect_height = im.cols / 45;
    for (int i = 0; i < contours.size(); i++)
    {
        std::vector<cv::Point> hull;
        cv::convexHull(cv::Mat(contours[i]), hull, false);
        cv::RotatedRect rect = cv::minAreaRect(cv::Mat(contours[i]));

        double aspect_ratio = rect.size.width / rect.size.height;
        if (rect.size.height < rect_height || rect.size.width < rect_width || aspect_ratio > 6.0 || aspect_ratio < 1.0/6.0)
        {
            filtered_contours.push_back(contours[i]);
            hulls.push_back(hull);
            cv::drawContours(edge_mask, hulls, hulls.size()-1, cv::Scalar(0), CV_FILLED);
        }
    }
    int element_size = 3;
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
                      cv::Size(2 * element_size + 1, 2 * element_size + 1),
                      cv::Point(element_size, element_size));
    cv::erode(edge_mask, edge_mask, element);
    cv::erode(edge_mask, edge_mask, element);
    cv::erode(edge_mask, edge_mask, element);
    cv::dilate(edge_mask, edge_mask, element);
    cv::dilate(edge_mask, edge_mask, element);
}

void ImageDifferencing::applyMask(const cv::Mat &in, const cv::Mat &mask, cv::Mat &out)
{
    in.copyTo(out, mask);
    cv::medianBlur(out, out, 3);
}

bool ImageDifferencing::clusterContourPartition(const cv::Mat &in, double distance_threshold, std::vector<std::vector<cv::Point> > &clusters, int min_area)
{
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(in, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));

    std::vector<cv::Point> contourPoints;
    for (int i = 0; i < contours.size(); i++)
    {
        if (cv::contourArea(contours[i]) < min_area)
        {
            continue;
        }
        contourPoints.insert(contourPoints.end(), contours[i].begin(), contours[i].end());
    }
    std::vector<int> labels;

    int number_of_labels = cv::partition(contourPoints, labels, IsNeighbour(distance_threshold));
    clusters.resize(number_of_labels);
    for (int i = 0; i < contourPoints.size(); i++)
    {
        clusters[labels[i]].push_back(contourPoints[i]);
    }
    return true;
}



void ImageDifferencing::getBoundingRectangles(const std::vector<std::vector<cv::Point> > &clusters, std::vector<cv::Rect> &bounding_rectangles)
{
    for (int i = 0; i < clusters.size(); i++)
    {
        cv::Rect rectangle = cv::boundingRect(cv::Mat(clusters[i]));
        bounding_rectangles.push_back(rectangle);
    }
}


void ImageDifferencing::drawBoundingRectangles(const std::vector<cv::Rect> &bounding_rectangles, cv::Mat &out)
{
    cv::Scalar red(0, 0, 255);
    for (int i = 0; i < bounding_rectangles.size(); i++)
    {
        cv::rectangle(out, bounding_rectangles[i].tl(), bounding_rectangles[i].br(), red, 2, 8, 0);
    }
}

float ImageDifferencing::percentile(const cv::Mat &im, double which_percentile)
{
    float* data = reinterpret_cast<float*>(im.data);
    int len = im.rows * im.cols;
    std::vector<float> vec(len);
    std::copy(data + 0, data + len, vec.begin());
    std::sort(vec.begin(), vec.end());
    return *(vec.begin() + int(vec.size() * which_percentile));
}
