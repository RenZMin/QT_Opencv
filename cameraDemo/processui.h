#ifndef PROCESSUI_H
#define PROCESSUI_H

#include <QWidget>
#include <opencv2/opencv.hpp>
#include <highgui.hpp>
#include "cv.h"
#include <cv.hpp>
#include<map>
#include<string>
using namespace cv;
using namespace std;
namespace Ui {
class processUi;
}

class processUi : public QWidget
{
    Q_OBJECT

public:
    explicit processUi(QWidget *parent = nullptr);
    ~processUi();

private slots:
    void showLoadedPicsL();

    void showLoadedPicsR();

    void steroMatch();

    void rectPicShow();

    void saveDisp(const string,const Mat&);

    void save3DXYZ(const string,const Mat&);

    //void onMouse(int event, int x, int y, int, void*);//鼠标获取三维坐标

//    void on_pushButton_7_clicked();

//    void on_loadPicButton_clicked();

    void on_loadRightPicButton_clicked();

    void on_loadCameraDataButton_clicked();

    void on_matchButton_clicked();

    void on_caliButton_clicked();

    void on_loadLeftPicButton_clicked();

    void on_quitButton_clicked();

    void F_Gray2Color(Mat gray_mat, Mat &color_mat);

    void F_mergeImg(Mat img1,Mat disp8,Mat& color_mat);

    void on_mergeButton_clicked();

    void on_adjustButton_clicked();

    void on_lastPicButton_clicked();

    void on_nextPicButton_clicked();

    void on_nextPicButton_2_clicked();

private:
    Ui::processUi *ui;
    QTimer *timer1;
    int numOfPic=0;//图片编号
    int picNum = 0;
    int imageWidth;                             //摄像头的分辨率
    int imageHeight;
    Size imageSize;// = Size(imageWidth, imageHeight);

    Mat rgbImageL, grayImageL;
    Mat rgbImageR, grayImageR;
    Mat rectifyImageL, rectifyImageR;//校正后的灰度图
    Mat rgbRectifyImageL, rgbRectifyImageR;//校正后RGB
    vector<Mat>rgbImageMatVecL;
    vector<Mat>rgbImageMatVecR;
    vector<Mat>grayImageMatVecL;
    vector<Mat>grayImageMatVecR;
    vector<Mat>imagerectifyMatVecL;
    vector<Mat>imagerectifyMatVecLCopy;
    vector<Mat>imagerectifyMatVecR;
    vector<Mat>canvasVec;//将校正后合并的图像存储
    vector<Mat>dispMatVec;//最初的CV_16s
    vector<Mat>disp8MatVec;//8位无符号的深度图向量，已经由最初的CV_16s转换成CV_8U，而且已经经过归一化

     vector<Mat>mergeMatVec;
       vector<Mat>weicaiseMatVec;

    vector<Mat>xyzMatVec;
    //map<int,Mat>xyzMatMap;
    Rect validROIL;//图像校正之后，会对图像进行裁剪，这里的validROI就是指裁剪之后的区域
    Rect validROIR;

    Mat mapLx, mapLy, mapRx, mapRy;     //映射表
    Mat Rl, Rr, Pl, Pr, Q;              //校正旋转矩阵R，投影矩阵P 重投影矩阵Q
    Mat xyz;              //三维坐标

    Point origin;         //鼠标按下的起始点
    Rect selection;      //定义矩形选框
    bool selectObject = false;    //是否选择对象

    int blockSize = 0, uniquenessRatio =0, numDisparities=0;
    Ptr<StereoBM> bm = StereoBM::create(16, 9);

    /*
    事先标定好的相机的参数
    fx 0 cx
    0 fy cy
    0 0  1
    */
    Mat cameraMatrixL;// = (Mat_<double>(3, 3) << 682.55880, 0, 384.13666,
        //0, 682.24569, 311.19558,
        //0, 0, 1);
    Mat distCoeffL;// = (Mat_<double>(5, 1) << -0.51614, 0.36098, 0.00523, -0.00225, 0.00000);

    Mat cameraMatrixR;// = (Mat_<double>(3, 3) << 685.03817, 0, 397.39092,
        //0, 682.54282, 272.04875,
        //0, 0, 1);
    Mat distCoeffR;// = (Mat_<double>(5, 1) << -0.46640, 0.22148, 0.00947, -0.00242, 0.00000);

    Mat T ;//= (Mat_<double>(3, 1) << -61.34485, 2.89570, -4.76870);//T平移向量
    Mat rec;// = (Mat_<double>(3, 1) << -0.00306, -0.03207, 0.00206);//rec旋转向量
    Mat R;//R 旋转矩阵






};

#endif // PROCESSUI_H
