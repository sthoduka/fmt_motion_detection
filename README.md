fmt_motion_detection uses the Fourier-Mellin transform for image registration and temporal differencing on the registered images to detect independent motions even in the presense of camera motion.

For images of size 320x240, the algorithm runs at approximately 11 Hz on an Intel Core i3 (1.7 GHz).

Dependencies
------------
* fftw3
* OpenCV 2.4
* Eigen
* Boost (only required in image_main for directory iteration)

Clone
------------
    git clone --recursive https://github.com/sthoduka/fmt_motion_detection.git

Compile
-------
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make

Run
---
    ./fmt_motion_detection_image <path to directory with images> [options]
    ./fmt_motion_detection_video <path to video> [options]

Options
-------
    -r M        scale down frame size by a factor of M
    -s          show intermediate outputs
    -p          start paused
    -fps N      set process frame rate to N (assumes camera frame rate is 30)
    -nomask     do not apply edge mask

Controls
--------
    spacebar  pause
    c         continue
    q         quit

Parameters
----------
Following are some of the parameters that can be tuned based on the application / environment.

* [`min_area`](https://github.com/sthoduka/fmt_motion_detection/blob/master/src/motion_detection.cpp#L74): defines the minimum size of the bounding boxes. Currently this is 8 times the number of columns in the image. If you think the output bounding boxes are too small, you can increase this.

* [`cluster_euclidean_threshold`](https://github.com/sthoduka/fmt_motion_detection/blob/master/src/motion_detection.cpp#L8): pixels in the contours which are closer than this threshold are considered to be part of one cluster. Currently it is set to a tenth of the number of columns in the image. If you think you are getting too many bounding boxes close to each other and want to combine them, then increase this threshold. Alternatively, perform non-maximum supression on the output list of bounding boxes.

* [`difference_intensity_threshold`](https://github.com/sthoduka/fmt_motion_detection/blob/master/src/motion_detection.cpp#L7): intensities in the difference image below this threshold are set to black in the thresholded image. Currently this is set to 40; i.e. if the intensity of the pixel is below 40, it is set to black, and anything between 40 and 255 are set to white in the thresholded image. This is important if your lighting conditions significantly different from the ones I tested with. If your images are darker, you might want to lower this threshold.

* [`rect_width and rect_height`](https://github.com/sthoduka/fmt_motion_detection/blob/master/src/image_differencing.cpp#L59): contours whose bounding rectangle is smaller in width or height than this threshold will be considered edges and masked out.

* [`aspect_ratio`](https://github.com/sthoduka/fmt_motion_detection/blob/master/src/image_differencing.cpp#L68): if the aspect ratio of the bounding rectangles of contours is less than 1/6 or greater than 6, it will be considered an edge and masked out.


Citation
--------

Please cite this work in your publications if you found it useful. Here is the BibTeX entry:

```
@inproceedings{Thoduka2017,
Title = {{Motion Detection in the Presence of Egomotion Using the Fourier-Mellin Transform}},
Author = {Thoduka, Santosh and Hegger, Frederik and Kraetzschmar, Gerhard K. and Pl{\"o}ger, Paul G.},
Booktitle = {Proceedings of the 21st RoboCup International Symposium},
Address = {Nagoya, Japan},
Keywords = {motion detection, mobile robots, egomotion compensation, fourier-mellin transform},
Timestamp = {2017.07.31},
Year = {2017}
}
```
