#ifndef DOUBLECALI_H
#define DOUBLECALI_H

#include <QWidget>
#include <opencv2/opencv.hpp>
#include <highgui.hpp>
#include "cv.h"
#include <cv.hpp>

using namespace cv;
using namespace std;

namespace Ui {
class DoubleCali;
}

class DoubleCali : public QWidget
{
    Q_OBJECT

public:
    explicit DoubleCali(QWidget *parent = nullptr);
    ~DoubleCali();

private slots:
    void on_doubleCalibButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void showPic();

    void showRectPic();

    void showInOne();

    void on_pushButton_6_clicked();

    void on_quitButton_clicked();

    void on_pushButton_7_clicked();

private:
    Ui::DoubleCali *ui;
    QTimer *timer2;
    QTimer *timer3;
    int imageWidth ;    								   //摄像头的分辨率
    int imageHeight ;
    int boardWidth ;               						   //横向的角点数目
    int boardHeight ;								       //纵向的角点数据
    int boardCorner;		                               //总的角点数据
    int frameNumber;								       //相机标定时需要采用的图像帧数
    int squareSize;								           //标定板黑白格子的大小 单位mm
    Size boardSize;

    Size imageSize = Size(imageWidth, imageHeight);
    Mat R,T,E,F;
    vector<Mat> rvecs;									    //旋转向量
    vector<Mat> tvecs;										//平移向量
    vector<vector<Point2f>> imagePointL;				    //左边摄像机所有照片角点的坐标集合,对应单目里的corner，
    vector<vector<Point2f>> imagePointR;					//右边摄像机所有照片角点的坐标集合
    vector<vector<Point3f>> objRealPoint;					//各副图像的角点的实际物理坐标集合
    int picNumShow=0;                                       //计数用，每次用完置零；

    vector<Point2f> cornerL;								//左边摄像机某一照片角点坐标集合
    vector<Point2f> cornerR;
    vector<cv::Mat> imageMatVecL;
    vector<cv::Mat> imagerectifyMatVecL;
    vector<cv::Mat> imageMatVecR;
    vector<cv::Mat> imagerectifyMatVecR;
    Mat Rl, Rr, Pl, Pr, Q;									//校正旋转矩阵R，投影矩阵P 重投影矩阵Q (下面有具体的含义解释）
    Mat mapLx,mapLy, mapRx, mapRy;	                        //映射表
    Mat rectifyImageL, rectifyImageR;                       //校正好的图像
    vector<Mat>  canvasVec;                                    //左右图像合并到同一张图像
    Rect validROIL, validROIR;                              //图像校正之后，会对图像进行裁剪，这里的validROI就是指裁剪之后的区域
    Mat cameraMatrixR, distCoeffR;
    Mat cameraMatrixL,distCoeffL ;

    /*以下是匹配所需变量*/

    Point origin;         //鼠标按下的起始点
    Rect selection;      //定义矩形选框
    bool selectObject = false;    //是否选择对象

    int blockSize = 0, uniquenessRatio =0, numDisparities=0;
    Ptr<StereoBM> bm = StereoBM::create(16, 9);

};

#endif // DOUBLECALI_H
