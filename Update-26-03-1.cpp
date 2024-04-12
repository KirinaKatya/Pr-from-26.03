#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void processVideo(VideoCapture cap, const string& output_path, int h1, int s1, int v1, int h2, int s2, int v2) {
    int width = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT));

    int codec = VideoWriter::fourcc('X', 'V', 'I', 'D');
    VideoWriter video(output_path, codec, 30, Size(width, height));

    namedWindow("Video", WINDOW_NORMAL);
    resizeWindow("Video", width, height);

    namedWindow("HSV video", WINDOW_NORMAL);
    resizeWindow("HSV video", width, height);

    while (true) {
        Mat frame;
        cap >> frame;

        if (frame.empty()) {
            cout << "End of video" << endl;
            break;
        }

        GaussianBlur(frame, frame, Size(5, 5), 0);

        Mat hsv;
        cvtColor(frame, hsv, COLOR_BGR2HSV);

        Scalar lower_white = Scalar(h1, s1, v1);
        Scalar upper_white = Scalar(h2, s2, v2);

        Mat mask;
        inRange(hsv, lower_white, upper_white, mask);

        Mat edges;
        Canny(mask, edges, 30, 90);

        dilate(edges, edges, Mat(), Point(-1, -1), 2);
        erode(edges, edges, Mat(), Point(-1, -1), 2);

        vector<vector<Point>> contours;
        findContours(edges, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        for (size_t i = 0; i < contours.size(); ++i) {
            double area = contourArea(contours[i]);
            if (area > 1000) {
                vector<Point> approx;
                approxPolyDP(contours[i], approx, arcLength(contours[i], true) * 0.02, true);
                if (approx.size() == 4) {
                    drawContours(frame, vector<vector<Point>>{approx}, 0, Scalar(0, 255, 0), 2);
                }
            }
        }

        imshow("Video", frame);
        imshow("HSV video", mask);

        video.write(frame);

        if (waitKey(27) == 'q')
            break;
    }

    cap.release();
    destroyAllWindows();

    video.release();
}

int main() {
    VideoCapture cap("C:/Users/Катя/Desktop/Python/video2.mp4");

    if (!cap.isOpened()) {
        cout << "Error opening video file" << endl;
        return -1;
    }

    int h1 = 100, s1 = 10, v1 = 140;
    int h2 = 255, s2 = 255, v2 = 255;

    processVideo(cap, "C:/Users/Катя/Desktop/Python/output.avi", h1, s1, v1, h2, s2, v2);

    return 0;
}
