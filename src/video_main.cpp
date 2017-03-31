#include <fmt_motion_detection/motion_detection.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/eigen.hpp>

#include <set>

int main(int argc, char **argv)
{
    cv::VideoCapture vidCap;
    cv::Mat im;

    std::string input_directory;
    if (argc < 2)
    {
        std::cout << "specify video file" << std::endl;
        return 1;
    }
    vidCap = cv::VideoCapture(argv[1]);
    if (!vidCap.isOpened())
    {
        std::cout << "Cannot open file " << argv[1] << std::endl;
        return 1;
    }

    vidCap >> im;

    bool resize = false;
    bool show_all_output = false;
    bool step_mode = false;
    bool use_edge_mask = true;
    int frame_rate = 30;
    if (argc > 2)
    {
        for (int i = 2; i < argc; i++)
        {
            if (strcmp(argv[i], "-r") == 0)
            {
                resize = true;
            }
            if (strcmp(argv[i], "-s") == 0)
            {
                show_all_output = true;
            }
            if (strcmp(argv[i], "-p") == 0)
            {
                step_mode = true;
            }
            if (strcmp(argv[i], "-fps") == 0)
            {
                frame_rate = atoi(argv[i+1]);
            }
            if (strcmp(argv[i], "-nomask") == 0)
            {
                use_edge_mask = false;
            }
        }
    }

    if (resize)
    {
        cv::resize(im, im, cv::Size(im.cols/2, im.rows/2));
    }

    MotionDetection md(im);
    md.setUseEdgeMask(use_edge_mask);

    int frame_number = 0;
    int initial_frame_rate = 30;
    int process_frame_rate = frame_rate;
    int divisor = initial_frame_rate / process_frame_rate;
    cv::Mat output;

    while(true)
    {
        frame_number++;
        vidCap >> im;

        if (im.empty())
        {
            std::cout << "done" << std::endl;
            break;
        }

        if (frame_number % divisor !=0) continue;

        if (resize)
        {
            cv::resize(im, im, cv::Size(im.cols/2, im.rows/2));
        }

        md.detectMotion(im, output, show_all_output);

        int c = cv::waitKey(1);
        // spacebar to pause, 'c' to continue
        if (c == 32 || step_mode)
        {
            step_mode = true;
            int x = cv::waitKey(0);
            if ((char)x == 'c')
            {
                step_mode = false;
            }
            else if ((char)x == 'q')
            {
                exit(0);
            }

        }
        else if ((char)c == 'q')
        {
            exit(0);
        }
    }
    return 0;
}
