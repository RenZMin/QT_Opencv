#include "doublecali.h"
#include "ui_doublecali.h"
#include <opencv2/opencv.hpp>
#include <highgui.hpp>
#include "cv.h"
#include <cv.hpp>
#include<QDebug>
#include<QTimer>
#include<QMessageBox>
DoubleCali::DoubleCali(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DoubleCali)
{
     ui->setupUi(this);
     this->setAttribute(Qt::WA_DeleteOnClose,true);
     ui->beforeRect_label_L->setScaledContents(true);//fit video to lable area
     ui->beforeRect_label_R->setScaledContents(true);
     ui->afterRect_label_L->setScaledContents(true);
     ui->afterRect_label_R->setScaledContents(true);
     ui->canvas_label->setScaledContents(true);
}

DoubleCali::~DoubleCali()
{
    delete ui;
}
QImage MatQImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

void DoubleCali::showPic()
 {

      if(picNumShow<frameNumber)
      {
         Mat matL = imagerectifyMatVecL[picNumShow];
         Mat matR = imagerectifyMatVecR[picNumShow];
         QImage matQimageL = MatQImage(matL);
         QImage matQimageR = MatQImage(matR);
         ui->beforeRect_label_L->setPixmap(QPixmap::fromImage(matQimageL));
         ui->beforeRect_label_R->setPixmap(QPixmap::fromImage(matQimageR));
         picNumShow++;
      }
      else
      {

          picNumShow = 0;
           qDebug()<<tr("stop pic");
          timer2->stop();
          QMessageBox::information(this,tr("提示："),tr("图片读取展示完毕!"),tr("确定"));



      }
 }

void DoubleCali::showRectPic()
{

    if(picNumShow<frameNumber)
    {

        /*经过remap之后，左右相机的图像已经共面并且行对准了
        */
        remap(imagerectifyMatVecL[picNumShow], imagerectifyMatVecL[picNumShow], mapLx, mapLy, INTER_LINEAR);
        QImage matQimagerectL = MatQImage(imagerectifyMatVecL[picNumShow]);
        ui->afterRect_label_L->setPixmap(QPixmap::fromImage(matQimagerectL));
        remap(imagerectifyMatVecR[picNumShow], imagerectifyMatVecR[picNumShow], mapRx, mapRy, INTER_LINEAR);
        QImage matQimagerectR = MatQImage(imagerectifyMatVecR[picNumShow]);
        ui->afterRect_label_R->setPixmap(QPixmap::fromImage(matQimagerectR));
        picNumShow++;
    }
    else
    {
        picNumShow = 0;
        qDebug()<<tr("stop");
        timer2->stop();
        QMessageBox::information(this,tr("提示："),tr("校正完毕!"),tr("确定"));
    }

}

void DoubleCali::showInOne()
{
    if(picNumShow<frameNumber)
    {
        Mat canvas;
        double sf;
        int w, h;
        sf = 600. / MAX(imageSize.width, imageSize.height);
        w = cvRound(imageSize.width * sf);
        h = cvRound(imageSize.height * sf);
        canvas.create(h, w * 2, CV_8UC3);
        /*左图像画到画布上*/
        Mat canvasPart = canvas(Rect(w*0, 0, w, h));//(w*0,0)是左上角的像素坐标 得到画布的一部分
        cv::resize(imagerectifyMatVecL[picNumShow], canvasPart, canvasPart.size(), 0, 0, INTER_AREA);		//把图像缩放到跟canvasPart一样大小
        Rect vroiL(cvRound(validROIL.x*sf), cvRound(validROIL.y*sf),				//获得被截取的区域
        cvRound(validROIL.width*sf),cvRound(validROIL.height*sf));
        rectangle(canvasPart, vroiL, Scalar(0, 0, 255), 3, 8);						//画上一个矩形
        /*右图像画到画布上*/
        canvasPart = canvas(Rect(w, 0, w, h));										//获得画布的另一部分
        cv::resize(imagerectifyMatVecR[picNumShow], canvasPart, canvasPart.size(), 0, 0, INTER_LINEAR);
        Rect vroiR(cvRound(validROIR.x*sf), cvRound(validROIR.y*sf),				//获得被截取的区域
        cvRound(validROIR.width*sf),cvRound(validROIR.height*sf));
        rectangle(canvasPart, vroiR, Scalar(0, 255, 0), 3, 8);
        /*画上对应的线条*/
        for (int i = 0; i < canvas.rows;i+=16)
            line(canvas, Point(0, i), Point(canvas.cols, i), Scalar(0, 255, 0), 1, 8);
        canvasVec.push_back(canvas);
        QImage canvasQImage = MatQImage(canvas);
        ui->canvas_label->setPixmap(QPixmap::fromImage(canvasQImage));
        imshow("rectified", canvas);
        picNumShow++;
    }
    else
    {
        picNumShow=0;
        timer2->stop();
        QMessageBox::information(this,tr("提示："),tr("校正图片合并完毕!"),tr("确定"));
    }
}



void DoubleCali::on_doubleCalibButton_clicked()
{
    FileStorage fileL(".\\leftData.xml",FileStorage::READ);
    FileStorage fileR(".\\rightData.xml",FileStorage::READ);
     FileStorage mapl(".\\mapl.xml",FileStorage::READ);
    /*
    事先标定好的左相机的内参矩阵
    fx 0 cx
    0 fy cy
    0 0  1
    */

    fileL["intrinsic"] >> cameraMatrixL;
    fileL["distortion_coeff"] >> distCoeffL;

    /*
    事先标定好的右相机的内参矩阵
    fx 0 cx
    0 fy cy
    0 0  1
    */

    fileR["intrinsic"] >> cameraMatrixR;
    fileR["distortion_coeff"] >> distCoeffR;
    //公共数据读取
    fileL["imageWidth"] >> imageWidth;
    fileL["imageHeight"] >> imageHeight;
    fileL["boardWidth"] >> boardWidth;
    fileL["boardHeight"] >> boardHeight;
    fileL["boardCorner"] >> boardCorner;
    fileL["frameNumber"] >> frameNumber;
    fileL["squareSize"] >> squareSize;
    fileL["boardSize"] >> boardSize;
    fileL["corners"] >> imagePointL;
    fileR["corners"] >> imagePointR;
    fileL["objRealPoint"] >> objRealPoint;
    fileL["imageMatVecL"]>>imageMatVecL;
    fileR["imageMatVecR"]>>imageMatVecR;
    fileL["imageMatVecL"]>>imagerectifyMatVecL;
    fileR["imageMatVecR"]>>imagerectifyMatVecR;
    imageSize = Size(imageWidth, imageHeight);
    /*
        标定摄像头
        由于左右摄像机分别都经过了单目标定
        所以在此处选择flag = CALIB_USE_INTRINSIC_GUESS
    */

    double rms = stereoCalibrate(objRealPoint, imagePointL, imagePointR,
            cameraMatrixL, distCoeffL,
            cameraMatrixR, distCoeffR,
            Size(imageWidth, imageHeight), R, T, E, F,
            CALIB_USE_INTRINSIC_GUESS,
            TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 100, 1e-5));
    qDebug()<< "Stereo Calibration done with RMS error = " << rms << endl;

   //展示修正前的左右照片
     timer2 = new QTimer(this);
     connect(timer2,SIGNAL(timeout()),this,SLOT(showPic()));
     timer2->start(1000);

}

void DoubleCali::on_pushButton_2_clicked()//校正
{
    /*
        立体校正的时候需要两幅图像共面并且行对准 以使得立体匹配更加的可靠
        使得两幅图像共面的方法就是把两个摄像头的图像投影到一个公共成像面上，这样每幅图像从本图像平面投影到公共图像平面都需要一个旋转矩阵R
        stereoRectify 这个函数计算的就是从图像平面投影都公共成像平面的旋转矩阵Rl,Rr。 Rl,Rr即为左右相机平面行对准的校正旋转矩阵。
        左相机经过Rl旋转，右相机经过Rr旋转之后，两幅图像就已经共面并且行对准了。
        其中Pl,Pr为两个相机的投影矩阵，其作用是将3D点的坐标转换到图像的2D点的坐标:P*[X Y Z 1]' =[x y w]
        Q矩阵为重投影矩阵，即矩阵Q可以把2维平面(图像平面)上的点投影到3维空间的点:Q*[x y d 1] = [X Y Z W]。其中d为左右两幅图像的时差
        */

    stereoRectify(cameraMatrixL, distCoeffL, cameraMatrixR, distCoeffR, imageSize, R, T, Rl, Rr, Pl, Pr, Q,
                      CALIB_ZERO_DISPARITY,0,imageSize,&validROIL,&validROIR);
    initUndistortRectifyMap(cameraMatrixL, distCoeffL, Rl, Pl, imageSize,CV_32FC1, mapLx, mapLy);
    initUndistortRectifyMap(cameraMatrixR, distCoeffR, Rr, Pr, imageSize, CV_32FC1, mapRx, mapRy);

    /*
        根据stereoRectify 计算出来的R 和 P 来计算图像的映射表 mapx,mapy
        mapx,mapy这两个映射表接下来可以给remap()函数调用，来校正图像，使得两幅图像共面并且行对准
        ininUndistortRectifyMap()的参数newCameraMatrix就是校正后的摄像机矩阵。在openCV里面，校正后的计算机矩阵Mrect是跟投影矩阵P一起返回的。
        所以我们在这里传入投影矩阵P，此函数可以从投影矩阵P中读出校正后的摄像机矩阵
        */
    timer2 = new QTimer(this);
    connect(timer2,SIGNAL(timeout()),this,SLOT(showRectPic()));
    timer2->start(1000);



}

void DoubleCali::on_pushButton_3_clicked()//匹配
{


}

void DoubleCali::on_pushButton_6_clicked()//把校正后的图像画到一幅图中
{


    timer2 = new QTimer(this);
    connect(timer2,SIGNAL(timeout()),this,SLOT(showInOne()));
    timer2->start(1000);



}

void DoubleCali::on_quitButton_clicked()
{
    this->close();
}

void DoubleCali::on_pushButton_7_clicked()//保存数据
{
    FileStorage doubleCameraInf(".\\doubleCameraData.xml",FileStorage::WRITE);
    doubleCameraInf<<"R"<<R;
    doubleCameraInf<<"T"<<T;
    doubleCameraInf<<"Rl"<<Rl;
    doubleCameraInf<<"Rr"<<Rr;
    doubleCameraInf<<"Pl"<<Pl;
    doubleCameraInf<<"Pr"<<Pr;
    doubleCameraInf<<"Q"<<Q;
//    doubleCameraInf<<"validROIL"<<validROIL;
//    doubleCameraInf<<"validROIL"<<validROIL;
    doubleCameraInf<<"mapLx"<<mapLx;
    doubleCameraInf<<"mapLy"<<mapLy;
    doubleCameraInf<<"mapRx"<<mapRx;
    doubleCameraInf<<"mapRy"<<mapRy;
    doubleCameraInf.release();
    QMessageBox::information(this,tr("提示："),tr("数据保存完毕!"),tr("确定"));
}
