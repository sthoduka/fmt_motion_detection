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
    -r          resize frames to half
    -s          show intermediate outputs
    -p          start paused
    -fps N      set process frame rate to N (assumes camera frame rate is 30)
    -nomask     do not apply edge mask

Controls
--------
    spacebar  pause
    c         continue
    q         quit
