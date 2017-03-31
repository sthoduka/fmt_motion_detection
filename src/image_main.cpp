#include <fmt_motion_detection/motion_detection.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/eigen.hpp>

#include <set>

#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;

int main(int argc, char **argv)
{
    cv::Mat im;

    std::string input_directory;
    if (argc < 2)
    {
        std::cout << "specify image directory" << std::endl;
        return 1;
    }
    input_directory = std::string(argv[1]);

    if (!bfs::exists(input_directory) || !bfs::is_directory(input_directory))
    {
        std::cerr << "directory does not exist: " << input_directory << std::endl;
        return 1;
    }

    std::set<std::string> sorted_images;
    bfs::directory_iterator end;
    for (bfs::directory_iterator iter(input_directory); iter != end; ++iter)
    {
        if (!bfs::is_directory(*iter))
        {
            sorted_images.insert(iter->path().string());
        }
    }
    bool resize = false;
    bool show_all_output = false;
    bool step_mode = false;
    bool use_edge_mask = true;
    int frames_to_skip = 0;
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
            if (strcmp(argv[i], "-j") == 0)
            {
                frames_to_skip = atoi(argv[i+1]);
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


    im = cv::imread(*(sorted_images.begin()), CV_LOAD_IMAGE_COLOR);

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

    std::set<std::string>::iterator iter = sorted_images.begin();
    std::advance(iter, frames_to_skip);

    while(iter != sorted_images.end())
    {
        ++iter;
        if (iter == sorted_images.end())
        {
            break;
        }
        frame_number++;
        im = cv::imread(*iter, CV_LOAD_IMAGE_COLOR);

        if (frame_number % divisor !=0) continue;

        if (im.empty())
        {
            std::cout << "done" << std::endl;
            break;
        }
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
