#ifndef CAMERAWINDOW_H
#define CAMERAWINDOW_H
#include"qdebug.h"
#include"QTimer"
#include "QFileDialog"
#include"caliform.h"
#include"doublecali.h"
#include"processui.h"
#include<QMessageBox>
#include <QMainWindow>
#include<iostream>
#include<string>
#include <sstream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;
namespace Ui {
class cameraWindow;
}

class cameraWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit cameraWindow(QWidget *parent = nullptr);
    ~cameraWindow();
private:
    Ui::cameraWindow *ui;
        caliForm *caliUi;//
        DoubleCali *doublecali;
        processUi *proUi;
        Mat frame,scrPicMat;
        VideoCapture capture;
        QImage  image,scrPic;
        QTimer *timer;
        double rate; //FPS
        VideoWriter writer;   //make a video record
        int picNum  = 0;
        bool videoFlag=false;



private slots:

    void nextFrame();

    void on_Camera_On_Button_clicked();

    void on_video_On_Button_clicked();

    void on_Process_Button_3_clicked();

    void on_StartRec_Button_clicked();
    void on_EndRec_Button_clicked();
    void on_CloseCamera_Button_clicked();
    void on_EndRec_Button_2_clicked();
    void on_SaveScrpic_Button_clicked();
    void on_Quit_Button_clicked();
    void on_Cali_Button_clicked();
    void on_Cali_Button_2_clicked();
    void on_process_Button_clicked();
    void on_process_Button_2_clicked();
};
//Mat QImageToMat(QImage image)
//{
//    Mat mat;
//    switch (image.format())
//    {
//    case QImage::Format_ARGB32:
//    case QImage::Format_RGB32:
//    case QImage::Format_ARGB32_Premultiplied:
//        mat = Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
//        break;
//    case QImage::Format_RGB888:
//        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
//        cv::cvtColor(mat, mat, CV_BGR2RGB);
//        break;
//    case QImage::Format_Indexed8:
//        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
//        break;
//    }
//    return mat;
//}


#endif // CAMERAWINDOW_H
