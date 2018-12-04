#ifndef CALIFORM_H
#define CALIFORM_H

#include <QWidget>
#include<QImage>
#include<QDebug>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <highgui.hpp>
#include "cv.h"
#include <cv.hpp>
#include <iostream>
#include <QCloseEvent>
using namespace cv;
using namespace std;
namespace Ui {
class caliForm;
}

class caliForm : public QWidget
{
    Q_OBJECT

public:
    explicit caliForm(QWidget *parent = nullptr);
    ~caliForm();

private slots:
    void showLoadedPics();
    void findDrawChessCorners();
    void caliUndis();
    void on_pushButton_clicked();
    void on_startCalibButton_clicked();


    void on_caliUndisButton_clicked();

    void on_saveDataButton_clicked();

    void on_saveDataButton_2_clicked();

    void on_lastButton_clicked();

    void on_removeDataButton_clicked();

    void on_DataButton_3_clicked();

private:
    Ui::caliForm *ui;
    caliForm *caliUiNew;
    vector<cv::Mat> imageMatVec;
    vector<cv::Mat> grayimageMatVec;
    vector<cv::Mat> undisimageMatvec;
     vector<cv::Mat> cornerimageMatvec;
    Mat rgbimage,grayimage,undisimage;
    QTimer *timer1;
    int picNum=0;
    int imageWidth = 0;                             //摄像头的分辨率
    int imageHeight = 0;
    int boardWidth = 0;                               //横向的角点数目
    int boardHeight = 0;                              //纵向的角点数据
    int boardCorner = boardWidth * boardHeight;       //总的角点数据
    int frameNumber = 0;                             //相机标定时需要采用的图像帧数
    int squareSize = 0;                              //标定板黑白格子的大小 单位mm
    int lastNumForCor=0;
    Size boardSize = Size(boardWidth, boardHeight);   //
    Mat intrinsic;                                          //相机内参数
    Mat distortion_coeff;                                   //相机畸变参数
    vector<Mat> rvecs;                                        //旋转向量
    vector<Mat> tvecs;                                        //平移向量
    vector<vector<Point2f>> corners;                        //各个图像找到的角点的集合 和objRealPoint 一一对应
    vector<vector<Point3f>> objRealPoint;                   //各副图像的角点的实际物理坐标集合
    vector<Point2f> corner;                                   //某一副图像找到的角点
    bool isFind,flag=false;
    /*计算标定板上模块的实际物理坐标*/
    void calRealPoint(vector<vector<Point3f>>& obj, int boardwidth,int boardheight, int imgNumber, int squaresize)
    {
    //  Mat imgpoint(boardheight, boardwidth, CV_32FC3,Scalar(0,0,0));
        vector<Point3f> imgpoint;
        for (int rowIndex = 0; rowIndex < boardheight; rowIndex++)
        {
            for (int colIndex = 0; colIndex < boardwidth; colIndex++)
            {
            //  imgpoint.at<Vec3f>(rowIndex, colIndex) = Vec3f(rowIndex * squaresize, colIndex*squaresize, 0);
                imgpoint.push_back(Point3f(rowIndex * squaresize, colIndex * squaresize, 0));
            }
        }
        for (int imgIndex = 0; imgIndex < imgNumber; imgIndex++)
        {
            obj.push_back(imgpoint);
        }
    }
    /*设置相机的初始参数 也可以不估计*/
    void guessCameraParam(void )
    {
        /*分配内存*/
        intrinsic.create(3, 3, CV_64FC1);
        distortion_coeff.create(5, 1, CV_64FC1);

        /*
        fx 0 cx
        0 fy cy
        0 0  1
        */
        intrinsic.at<double>(0,0) = 256.8093262;   //fx
        intrinsic.at<double>(0, 2) = 160.2826538;   //cx
        intrinsic.at<double>(1, 1) = 254.7511139;   //fy
        intrinsic.at<double>(1, 2) = 127.6264572;   //cy

        intrinsic.at<double>(0, 1) = 0;
        intrinsic.at<double>(1, 0) = 0;
        intrinsic.at<double>(2, 0) = 0;
        intrinsic.at<double>(2, 1) = 0;
        intrinsic.at<double>(2, 2) = 1;

        /*
        k1 k2 p1 p2 p3
        */
        distortion_coeff.at<double>(0, 0) = -0.193740;  //k1
        distortion_coeff.at<double>(1, 0) = -0.378588;  //k2
        distortion_coeff.at<double>(2, 0) = 0.028980;   //p1
        distortion_coeff.at<double>(3, 0) = 0.008136;   //p2
        distortion_coeff.at<double>(4, 0) = 0;          //p3

    }
};

#endif // CALIFORM_H
