#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

int main(){

    Mat myImage;
    namedWindow("lab5_evm");
    VideoCapture cap(0);
    TickMeter tm_frame;
    TickMeter tm_full;
    TickMeter tm_enter;
    TickMeter tm_morph;
    TickMeter tm_show;
    double full_time_enter = 0;
    double full_time_morph = 0;
    double full_time_show = 0;

    if (!cap.isOpened()){
        cout << "No video stream detected" << endl;
        return -1;
    }

    int frame_count = 0;
    int frame_count_shown = frame_count;
    int full_frame_count = frame_count;
    double time_count = 0;

    tm_full.start();
    while (true){
        tm_frame.start();

        tm_enter.start();
        cap >> myImage;
        if (myImage.empty()){
            break;
        }
        tm_enter.stop();
        full_time_enter += tm_enter.getTimeSec();
        tm_enter.reset();

        tm_morph.start();
        flip(myImage, myImage, 0);
        cvtColor(myImage, myImage, COLOR_BGR2GRAY);
        tm_morph.stop();
        full_time_morph += tm_morph.getTimeSec();
        tm_morph.reset();

        tm_frame.stop();

        frame_count++;
        time_count += tm_frame.getTimeSec();
        tm_frame.reset();
        cout << time_count << endl;

        if (time_count >= 1.0){
            frame_count_shown = frame_count;
            frame_count = 0;
            time_count = 0;
        }

        tm_frame.start();

        putText(myImage, to_string(frame_count_shown), cv::Point(10, myImage.rows / 2), cv::FONT_HERSHEY_DUPLEX, 1.0, CV_RGB(118, 185, 0), 2);

        double fps = cap.get(CAP_PROP_FPS);
        putText(myImage, to_string(fps), cv::Point(10, myImage.rows / 1.5), cv::FONT_HERSHEY_DUPLEX, 1.0, CV_RGB(118, 185, 0), 2);

        tm_show.start();
        imshow("lab5_evm", myImage);
        tm_show.stop();
        full_time_show += tm_show.getTimeSec();
        tm_show.reset();

        char c = (char)waitKey(33);
        if (c == 27){ 
            break;
        }

        full_frame_count++;

        tm_frame.stop();
        time_count += tm_frame.getTimeSec();
        tm_frame.reset();
    }
    tm_full.stop();
    cout << "Time working: " << tm_full.getTimeSec() << endl;

    double average_fps = full_frame_count / tm_full.getTimeSec();
    cout << "average fps: " << average_fps << endl;

    double time_sum = full_time_enter + full_time_morph + full_time_show;
    double percent = time_sum / 100;
    cout << (full_time_enter / percent) << "%" << endl << (full_time_morph / percent) << "%" << endl << (full_time_show / percent) << "%" << endl;

    cap.release();
    return 0;
}