#include "processui.h"
#include "ui_processui.h"
#include <opencv2/opencv.hpp>
#include <highgui.hpp>
#include "cv.h"
#include <cv.hpp>
#include <iostream>
#include<QFileDialog>
#include<QMessageBox>
#include<QDebug>
#include<QTimer>
#include<cmath>
processUi::processUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::processUi)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose,true);
    ui->beforeRect_label_L->setScaledContents(true);
    ui->beforeRect_label_R->setScaledContents(true);
    ui->afterRect_label_L->setScaledContents(true);
    ui->afterRect_label_R->setScaledContents(true);
    ui->canvas_label->setScaledContents(true);
    ui->match_label->setScaledContents(true);
    ui->d3label->setScaledContents(true);
}

processUi::~processUi()
{
    delete ui;
}
QImage MattoImage(const cv::Mat& mat)
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

void processUi::F_Gray2Color(Mat gray_mat, Mat& color_mat)
{
    color_mat = Mat::zeros(gray_mat.size(),CV_8UC3);
    int rows = color_mat.rows,cols=color_mat.cols;
    Mat red = Mat(gray_mat.rows,gray_mat.cols,CV_8U);
    Mat green = Mat(gray_mat.rows,gray_mat.cols,CV_8U);
    Mat blue = Mat(gray_mat.rows,gray_mat.cols,CV_8U);
    Mat mask = Mat(gray_mat.rows,gray_mat.cols,CV_8U);
    subtract(gray_mat,Scalar(255),blue); //blue(I) = 255 - gray(I)
    red = gray_mat.clone();// red(I) = gray(I)
    green = gray_mat.clone();
    compare(green,128,mask,CMP_GE);// green(I) = gray(I),if gray(I) < 128
    subtract(green,Scalar(255),green,mask);//green(I) = 255 - gray(I), if gray(I) >= 128
    convertScaleAbs(green,green,2.0,0/*2.0*/);
    vector<Mat> vec;
    vec.push_back(red);
    vec.push_back(green);
    vec.push_back(blue);
    merge(vec,color_mat);


}
/*进行图像融合，将左侧相机拍摄的校正图像（单通道）和视差图的灰度图分别作为3通道图像的两个通道，另外一个通道设置为0，
 * 可以形象地看出在原始图像的哪些区域可以实现立体匹配。融合后的图像，其中蓝色背景是左侧相机拍摄图像经过立体校正后的图像，
红色部分是视差图，可以看出在蓝色原始图像的强纹理区域可以实现立体匹配和视差计算。该特征是由BM立体匹配算法本身的属性
决定的，BM算法本身不适用于没有纹理的区域*/
void processUi::F_mergeImg(Mat img1,Mat disp8, Mat &color_mat)
{
     color_mat = Mat::zeros(img1.size(),CV_8UC3);
     cvtColor(img1,img1,CV_BGR2GRAY);//将输入的三通道图像变成单通道
     Mat red = img1.clone();
     Mat green =  disp8.clone();
     Mat blue = Mat::zeros(img1.size(),CV_8UC1);
     vector<Mat> mergeMatVec;
     mergeMatVec.push_back(red);
     mergeMatVec.push_back(blue);
     mergeMatVec.push_back(green);
     merge(mergeMatVec,color_mat);

 }


void processUi::showLoadedPicsL()
{
    if(picNum<rgbImageMatVecL.size())
    {
        Mat rgbImageL=rgbImageMatVecL[picNum];
        QImage rgbImageQImageL = MattoImage(rgbImageMatVecL[picNum]);
        QString widthStr= QString::number((rgbImageMatVecL[picNum].size().width));
        QString hightStr= QString::number((rgbImageMatVecL[picNum].size().height));
        ui->beforeRect_label_L->setPixmap(QPixmap::fromImage(rgbImageQImageL));
        ui->lineEdit_2->setText(widthStr+"X"+hightStr);
        ui->lineEdit_4->setText(QString::number(picNum+1));
        //cvtColor(rgbImageMatVecL[picNum],grayImageMatVecL[picNum],CV_BGR2GRAY);
       //imshow("rgbImageMatVecL"+to_string(picNum),rgbImageMatVecL[picNum]);
        //imshow("grayImageMatVecL"+to_string(picNum),grayImageMatVecL[picNum]);

        picNum++;
    }

    else
    {
        if(picNum!=0)
        {
            QMessageBox::information(NULL, "提示：","左视图片载入完毕",tr("确定"));
            timer1->stop();
            picNum=0;
        }
    }
}
void processUi::showLoadedPicsR()
{
    if(picNum<rgbImageMatVecR.size())
    {
        Mat rgbImageR=rgbImageMatVecR[picNum];
        QImage rgbImageQImageR = MattoImage(rgbImageMatVecR[picNum]);
        QString widthStr= QString::number((rgbImageMatVecR[picNum].size().width));
        QString hightStr= QString::number((rgbImageMatVecR[picNum].size().height));
        ui->beforeRect_label_R->setPixmap(QPixmap::fromImage(rgbImageQImageR));
        ui->lineEdit_2->setText(widthStr+"X"+hightStr);
        ui->lineEdit_4->setText(QString::number(picNum+1));
        //imshow("rrrrllinr"+to_string(picNum),grayImageMatVecL[picNum]);
        picNum++;
    }

    else
    {
        if(picNum!=0)
        {
            QMessageBox::information(NULL, "提示：","右视图片载入完毕",tr("确定"));
            timer1->stop();
            picNum=0;
        }
    }
}

void processUi::rectPicShow()
{
    if(picNum<rgbImageMatVecL.size())
    {
        ui->lineEdit_4->setText(QString::number(picNum+1));
        stereoRectify(cameraMatrixL, distCoeffL, cameraMatrixR, distCoeffR, imageSize, R, T, Rl, Rr, Pl, Pr, Q, CALIB_ZERO_DISPARITY,
               0, imageSize, &validROIL, &validROIR);
        cvtColor(rgbImageMatVecL[picNum],grayImageL,CV_BGR2GRAY);
        cvtColor(rgbImageMatVecR[picNum],grayImageR,CV_BGR2GRAY);

        remap(grayImageL, rectifyImageL, mapLx, mapLy, INTER_LINEAR);
        remap(grayImageR, rectifyImageR, mapRx, mapRy, INTER_LINEAR);

//        remap(grayImageMatVecL[picNum], rectifyImageL, mapLx, mapLy, INTER_LINEAR);
//        remap(grayImageMatVecR[picNum], rectifyImageR, mapRx, mapRy, INTER_LINEAR);

        cvtColor(rectifyImageL, rgbRectifyImageL, CV_GRAY2BGR);  //伪彩色图,便与存储，保证在其他地方访问时不变，如果直接访问单通道的Mat，前后图像会不一样（不知道为啥，写代码发现的）
        cvtColor(rectifyImageR, rgbRectifyImageR, CV_GRAY2BGR);
        Mat tempL = rgbRectifyImageL.clone();
        Mat tempR = rgbRectifyImageR.clone();
        imagerectifyMatVecL.push_back(tempL);
        imagerectifyMatVecR.push_back(tempR);
        //imagerectifyMatVecLCopy.push_back(temp);
        //imshow("imagerectifyMatVecL"+to_string(picNum),imagerectifyMatVecL[picNum]);
        //imagerectifyMatVecR.push_back(rgbRectifyImageR);
        QImage rgbRectifyQImageL = MattoImage(tempL);
        QImage rgbRectifyQImageR = MattoImage(tempR);
        ui->afterRect_label_L->setPixmap(QPixmap::fromImage(rgbRectifyQImageL));
        ui->afterRect_label_R->setPixmap(QPixmap::fromImage(rgbRectifyQImageR));
        /*显示到一幅图上*/
        Mat canvas;
        double sf;
        int w, h;
        sf = 600. / MAX(imageSize.width, imageSize.height);
        w = cvRound(imageSize.width * sf);
        h = cvRound(imageSize.height * sf);
        canvas.create(h, w * 2, CV_8UC3);
        /*左图像画到画布上*/
        Mat canvasPart = canvas(Rect(w*0, 0, w, h));//(w*0,0)是左上角的像素坐标 得到画布的一部分
        cv::resize(rgbRectifyImageL, canvasPart, canvasPart.size(), 0, 0, INTER_AREA);		//把图像缩放到跟canvasPart一样大小
        Rect vroiL(cvRound(validROIL.x*sf), cvRound(validROIL.y*sf),				//获得被截取的区域
        cvRound(validROIL.width*sf),cvRound(validROIL.height*sf));
        rectangle(canvasPart, vroiL, Scalar(0, 0, 255), 3, 8);						//画上一个矩形
        /*右图像画到画布上*/
        canvasPart = canvas(Rect(w, 0, w, h));										//获得画布的另一部分
        cv::resize(rgbRectifyImageR, canvasPart, canvasPart.size(), 0, 0, INTER_LINEAR);
        Rect vroiR(cvRound(validROIR.x*sf), cvRound(validROIR.y*sf),				//获得被截取的区域
        cvRound(validROIR.width*sf),cvRound(validROIR.height*sf));
        rectangle(canvasPart, vroiR, Scalar(0, 255, 0), 3, 8);
        /*画上对应的线条*/
        for (int i = 0; i < canvas.rows;i+=16)
            line(canvas, Point(0, i), Point(canvas.cols, i), Scalar(0, 255, 0), 1, 8);
        Mat canvastemp = canvas.clone();
        canvasVec.push_back(canvastemp);
        //canvasVec.push_back(canvas);
        QImage canvasQImage = MattoImage(canvastemp);
        ui->canvas_label->setPixmap(QPixmap::fromImage(canvasQImage));
//        imshow("grayImageMatVecL[picNum]"+to_string(picNum),grayImageMatVecL[picNum]);
//          imshow("rgbImageMatVecL[picNum]"+to_string(picNum),rgbImageMatVecL[picNum]);//rgbImageMatVecR
//       imshow("rectifyImageL"+to_string(picNum), rectifyImageL);
//         imshow("rgbRectifyImageL"+to_string(picNum), rgbRectifyImageL);
//        imshow("rectified"+to_string(picNum), canvas);
        //imshow("imagerectifyMatVecLL"+to_string(picNum),imagerectifyMatVecL[picNum]);
        picNum++;
    }
    else
    {

        timer1->stop();
        picNum=0;
        QMessageBox::information(NULL, tr("提示："),tr("校正完毕"),tr("确定"));

    }
}

void processUi::saveDisp(const string filename,const Mat& mat)
{
  ofstream fp(filename,ios::out);
  fp<<mat.rows<<endl;
  fp<<mat.cols<<endl;
  for(int y=0; y<mat.rows;y++)
  {
      for(int x=0; x<mat.cols;x++)
      {
         double disp = mat.at<short>(y,x);//行优先，这里视差矩阵是CV_16S格式的，所以用short类型读取
         fp<<disp<<endl;//若视差矩阵是CV_32F格式的，则用float类型读取
      }

   }
  fp.close();
  //FileStorage

}

void processUi::save3DXYZ(const string filename,const Mat& mat)
{
   const double max_z = 1.0e4;//允许测量的最大深度值，也即是距离
   ofstream fp(filename);
   if(!fp.is_open())
   {
       QMessageBox::warning(NULL, "警告：","打开文件失败",tr("确定"));
       fp.close();
       return;
   }
   else
   {
     for(int y = 0;y<mat.rows;y++)
     {
         for(int x = 0;x<mat.cols;x++)
         {
             Vec3f point = mat.at<Vec3f>(y,x);//三通道浮点数
             if(fabs(point[2]-max_z)<FLT_EPSILON||fabs(point[2]>max_z))
             {
                 fp<<point[0]<<" "<<point[1]<<" "<<point[2]<<endl;
             }
         }
     }
     fp.close();
   }
}

//void processUi::onMouse(int event, int x, int y, int, void*)
//{
//    if (selectObject)
//        {
//            selection.x = MIN(x, origin.x);
//            selection.y = MIN(y, origin.y);
//            selection.width = std::abs(x - origin.x);
//            selection.height = std::abs(y - origin.y);
//        }

//    switch (event)
//    {
//        case EVENT_LBUTTONDOWN:   //鼠标左按钮按下的事件
//        {
//            origin = Point(x, y);
//            selection = Rect(x, y, 0, 0);//rect(x, y, width, height);
//            selectObject = true;
//            Vec3f point = xyz.at<Vec3f>(origin);
//            QString xyz3Dpointx = QString::number(point[0]);
//            QString xyz3Dpointy = QString::number(point[1]);
//            QString xyz3Dpointz = QString::number(point[2]);
//            ui->lineEdit_3->setText(tr("(")+xyz3Dpointx+","+xyz3Dpointy+","+xyz3Dpointz+")");
//            //cout << origin <<"in world coordinate is: " << xyz.at<Vec3f>(origin) << endl;
//            break;
//         }
//         case EVENT_LBUTTONUP:   //鼠标左按钮释放的事件
//         {
//            selectObject = false;
//            if (selection.width > 0 && selection.height > 0)
//            break;
//         }
//    }



//}

/*立体匹配算法,双目相机测距，世界坐标系与左相机坐标系重合*/
void processUi::steroMatch()
{
    if(picNum<imagerectifyMatVecL.size())
    {
        ui->lineEdit_4->setText(QString::number(picNum+1));
        Mat grayRectImageL,grayRectImageR;
        cvtColor(imagerectifyMatVecL[picNum],grayRectImageL,CV_BGR2GRAY);
        imshow("imagerectifyMatVecLafter"+to_string(picNum),imagerectifyMatVecLCopy[picNum]);
        cvtColor(imagerectifyMatVecR[picNum],grayRectImageR,CV_BGR2GRAY);
        bm->setBlockSize(9);//(2*blockSize+5);     //SAD窗口大小，5~21之间为宜
        bm->setROI1(validROIL);
        bm->setROI2(validROIR);
        bm->setPreFilterCap(31);
        numDisparities= ((imageSize.width/8) + 15) & -16;//?
        bm->setMinDisparity(0);  //最小视差，默认值为0, 可以是负值，int型
        bm->setNumDisparities(numDisparities);//(numDisparities*16+16);//视差窗口，即最大视差值与最小视差值之差,窗口大小必须是16的整数倍，int型
        bm->setTextureThreshold(10);
        bm->setUniquenessRatio(5);//uniquenessRatio主要可以防止误匹配
        bm->setSpeckleWindowSize(100);
        bm->setSpeckleRange(32);
        bm->setDisp12MaxDiff(-1);
        Mat disp, disp8,dispTemp,disp8Temp,weicaisediap8Temp,mergeImgTemp,xyzTemp;//?
        bm->compute(grayRectImageL, grayRectImageR, disp);//输入图像必须为灰度图
        disp.convertTo(disp8, CV_8U, 255 / (numDisparities * 16.));//计算出的视差是CV_16S格式
        reprojectImageTo3D(disp, xyz, Q, true); //在实际求距离时，ReprojectTo3D出来的X / W, Y / W, Z / W都要乘以16(也就是W除以16)，才能得到正确的三维坐标信息。
        xyz = xyz * 16;//xyz矩阵存储的是三维坐标数据，而不是 RGB 颜色值，所以是不能调用cvShowImage() 或者 OpenCV2.1 版的 imshow() 等函数来显示这个矩阵，
        cv::normalize(disp8,disp8,0,255,CV_MINMAX);        
        //xyzMatMap[picNum]=xyz;//向Map里添加数据
        imshow("disparity", disp8);
        Mat weicaisediap8=disp8;
        F_Gray2Color(disp8,weicaisediap8);       
        //QImage xyz3dQImage = MattoImage(xyz);
        QImage disp8QImage = MattoImage(weicaisediap8);
        Mat mergeImg;//视差图与原图像校正后的左视图的融合
        F_mergeImg(imagerectifyMatVecL[picNum],disp8,mergeImg);       
        QImage mergeQImage = MattoImage(mergeImg);
        ui->match_label->setPixmap(QPixmap::fromImage(disp8QImage));//显示伪彩色图
        ui->d3label->setPixmap(QPixmap::fromImage(mergeQImage));
        dispTemp = disp.clone();
        disp8Temp = disp8.clone();
        weicaisediap8Temp = weicaisediap8.clone();
        mergeImgTemp = mergeImg.clone();
        xyzTemp = xyz.clone();
        dispMatVec.push_back(dispTemp);
        disp8MatVec.push_back(disp8Temp);
        weicaiseMatVec.push_back(weicaisediap8Temp);
        mergeMatVec.push_back(mergeImgTemp);
        xyzMatVec.push_back(xyzTemp);
        picNum++;
        numOfPic++;

        //ui->d3label->setPixmap(QPixmap::fromImage(xyz3dQImage));
        //OpenCV主要只支持单通道和3通道的图像，并且此时要求其深度为8bit和16bit无符号(即CV_16U)，

    }
    else
    {
        QMessageBox::information(NULL, tr("提示："),tr("匹配完毕"),tr("确定"));
        timer1->stop();
        picNum=0;
    }
}

void processUi::on_loadLeftPicButton_clicked()
{
    ui->loadRightPicButton->setEnabled(true);
    QList<QString> filename =QFileDialog::getOpenFileNames(this,tr("Open image File"),"/",tr("image Files(*.png *.jpg *.bmp);;All files (*.*)"));
    qDebug()<< filename.size();
    int i =0;
    for(auto &file : filename)
    {

        rgbImageL=imread(file.toStdString(),1);
        imageHeight = rgbImageL.size().height;
        imageWidth = rgbImageL.size().width;
        imageSize = Size(imageWidth,imageHeight);
        if(rgbImageL.empty())
        {
            if(!QMessageBox::warning(this,tr("提示"),tr("图片载入失败,其中一张为空图请检查"),tr("退出")))
                 this->close();
        }
        //cvtColor(rgbImageL,grayImageL,CV_BGR2GRAY);
        rgbImageMatVecL.push_back(rgbImageL);
        //grayImageMatVecL.push_back(grayImageL);
        //imshow("grY"+to_string(i),grayImageMatVecL[i]);
        ui->lineEdit->setText(QString::number(rgbImageMatVecL.size()));
        i++;

    }
    timer1= new QTimer(this);
    connect(timer1,SIGNAL(timeout()),this,SLOT(showLoadedPicsL()));
    timer1->start(1000);

}

void processUi::on_loadRightPicButton_clicked()
{
    QList<QString> filename =QFileDialog::getOpenFileNames(this,tr("Open image File"),"/",tr("image Files(*.png *.jpg *.bmp);;All files (*.*)"));
    qDebug()<< filename.size();
    for(auto &file : filename)
    {

        rgbImageR=imread(file.toStdString(),1);
        imageHeight = rgbImageR.size().height;
        imageWidth = rgbImageR.size().width;
        if(rgbImageR.empty())
        {
            if(!QMessageBox::warning(this,tr("提示"),tr("图片载入失败,其中一张为空图请检查"),tr("退出")))
                 this->close();
        }
        //cvtColor(rgbImageR,grayImageR,CV_BGR2GRAY);
        rgbImageMatVecR.push_back(rgbImageR);
        //grayImageMatVecR.push_back(grayImageR);

    }
    if(rgbImageMatVecL.size()!=rgbImageMatVecR.size())
    {
        QMessageBox::warning(this,tr("警告"),tr("左右视图数量不同，请按对应顺序重新载入"));
        ui->loadCameraDataButton->setDisabled(true);
        ui->matchButton->setDisabled(true);
        ui->caliButton->setDisabled(true);
    }
    else
    {
        timer1= new QTimer(this);
        connect(timer1,SIGNAL(timeout()),this,SLOT(showLoadedPicsR()));
        timer1->start(1000);
    }
}

void processUi::on_loadCameraDataButton_clicked()
{
    FileStorage fileL(".\\leftData.xml",FileStorage::READ);
    FileStorage fileR(".\\rightData.xml",FileStorage::READ);
    FileStorage fileDouble(".\\doubleCameraData.xml",FileStorage::READ);
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
    //双目数据读取
    fileDouble["T"]>>T;
    fileDouble["R"]>>R;
    fileDouble["mapLx"] >> mapLx;
    fileDouble["mapLy"] >> mapLy;
    fileDouble["mapRx"] >> mapRx;
    fileDouble["mapRy"] >> mapRy;

    fileL.release();
    fileR.release();
    fileDouble.release();
    QMessageBox::warning(this,tr("提示"),tr("相机数据载入完毕"),tr("确定"));
}

void processUi::on_caliButton_clicked()
{
    /*立体校正，获取R,T,Rl,Rr,Pl,Pr,Q,mapLx,MapLy,MapRx,MapRy,这里由之前标定双目摄像头时保存的数据直接获取*/
    timer1= new QTimer(this);
    connect(timer1,SIGNAL(timeout()),this,SLOT(rectPicShow()));
    timer1->start(2000);
    int i =0;
    for(auto & Mat :imagerectifyMatVecL)
    {
      imshow("imagerectifyMatVecL12"+to_string(i++),imagerectifyMatVecL[i++]);
    }

}

void processUi::on_matchButton_clicked()
{
    timer1= new QTimer(this);
    connect(timer1,SIGNAL(timeout()),this,SLOT(steroMatch()));
    timer1->start(5000);

}



void processUi::on_quitButton_clicked()
{

    this->close();
    //delete ui;
}

void processUi::on_mergeButton_clicked()//用于保存数据了
{

  for(int num = 0;num<dispMatVec.size();num++)
  {
     string filename =".\\图"+ to_string(num+1)+"视差数据.txt";
     saveDisp(filename,dispMatVec[num]);
  }
  for(int num = 0;num<xyzMatVec.size();num++)
  {
     string filename =".\\图"+ to_string(num+1)+"三维坐标数据.txt";
     saveDisp(filename,xyzMatVec[num]);
  }
  QMessageBox::information(this,tr("提示"),tr("视差数据和三维坐标数据已保存"),tr("确定"));
}

void processUi::on_adjustButton_clicked()
{
    int i = 0;
    for(auto &mat : disp8MatVec)
        imshow("disp8MatVec"+to_string(i++),mat);
}

void processUi::on_lastPicButton_clicked()
{

    picNum=numOfPic-1;

    if(picNum==0)
        QMessageBox::warning(NULL,tr("警告"),tr("已经是第一张"),tr("确定"));
    else
    {

        numOfPic = numOfPic-1;
        ui->lineEdit_4->setText(QString::number(numOfPic));
        QImage rgbImageQImageL = MattoImage(rgbImageMatVecL[picNum-1]);//
        QImage rgbImageQImageR = MattoImage(rgbImageMatVecR[picNum-1]);//
        QImage rgbRectifyQImageL = MattoImage(imagerectifyMatVecL[picNum-1]);
        QImage rgbRectifyQImageR = MattoImage(imagerectifyMatVecR[picNum-1]);
        QImage disp8QImage = MattoImage(disp8MatVec[picNum-1]);
        QImage canvasQImage = MattoImage(canvasVec[picNum-1]);//
        QImage weicaise =MattoImage(weicaiseMatVec[picNum-1]);
        QImage mergeQImage = MattoImage(mergeMatVec[picNum-1]);
        ui->beforeRect_label_L->setPixmap(QPixmap::fromImage(rgbImageQImageL));
        ui->beforeRect_label_R->setPixmap(QPixmap::fromImage(rgbImageQImageR));
        ui->afterRect_label_L->setPixmap(QPixmap::fromImage(rgbRectifyQImageL));
        ui->afterRect_label_R->setPixmap(QPixmap::fromImage(rgbRectifyQImageR));
        ui->canvas_label->setPixmap(QPixmap::fromImage(canvasQImage));
        ui->match_label->setPixmap(QPixmap::fromImage(weicaise));//伪彩色图
        ui->d3label->setPixmap(QPixmap::fromImage(mergeQImage));
    }

}

void processUi::on_nextPicButton_clicked()
{
    picNum=numOfPic+1;

    if(picNum==(rgbImageMatVecL.size()+1))
        QMessageBox::warning(NULL,tr("警告"),tr("已经是最后一张"),tr("确定"));
    else
    {

        numOfPic = numOfPic+1;
        ui->lineEdit_4->setText(QString::number(numOfPic));
        QImage rgbImageQImageL = MattoImage(rgbImageMatVecL[picNum-1]);
        QImage rgbImageQImageR = MattoImage(rgbImageMatVecR[picNum-1]);
        QImage rgbRectifyQImageL = MattoImage(imagerectifyMatVecL[picNum-1]);
        QImage rgbRectifyQImageR = MattoImage(imagerectifyMatVecR[picNum-1]);
        QImage disp8QImage = MattoImage(disp8MatVec[picNum-1]);
        QImage canvasQImage = MattoImage(canvasVec[picNum-1]);
        QImage weicaise =MattoImage(weicaiseMatVec[picNum-1]);
        QImage mergeQImage = MattoImage(mergeMatVec[picNum-1]);
        ui->beforeRect_label_L->setPixmap(QPixmap::fromImage(rgbImageQImageL));
        ui->beforeRect_label_R->setPixmap(QPixmap::fromImage(rgbImageQImageR));
        ui->afterRect_label_L->setPixmap(QPixmap::fromImage(rgbRectifyQImageL));
        ui->afterRect_label_R->setPixmap(QPixmap::fromImage(rgbRectifyQImageR));
        ui->canvas_label->setPixmap(QPixmap::fromImage(canvasQImage));
        ui->match_label->setPixmap(QPixmap::fromImage(weicaise));//伪彩色图
        ui->d3label->setPixmap(QPixmap::fromImage(mergeQImage));

    }
}

void processUi::on_nextPicButton_2_clicked()
{
    timer1= new QTimer(this);
    connect(timer1,SIGNAL(timeout()),this,SLOT(steroMatch()));
    timer1->start(5000);
}
