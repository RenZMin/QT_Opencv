#include "caliform.h"
#include "ui_caliform.h"
#include <opencv2/opencv.hpp>
#include <highgui.hpp>
#include "cv.h"
#include <cv.hpp>
#include <iostream>
#include<QFileDialog>
#include<QMessageBox>
#include<QDebug>
#include<QTimer>
using namespace std;
using namespace cv;
caliForm::caliForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::caliForm)
{
    ui->setupUi(this);
    ui->Corner_label->setScaledContents(true);
    ui->ini_label->setScaledContents(true);
    ui->cor_label->setScaledContents(true);
    ui->undis_label->setScaledContents(true);
    this->setAttribute(Qt::WA_DeleteOnClose,true);
}

caliForm::~caliForm()
{
    delete ui;
}
QImage Mat2QImage(const cv::Mat& mat)
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

void caliForm::showLoadedPics()
{
    if(picNum<imageMatVec.size())
    {
        Mat tempimage=imageMatVec[picNum];
        Mat tempgrayimage = grayimageMatVec[picNum];
        cvtColor(tempimage,tempgrayimage,CV_BGR2GRAY);
        QImage rgbimageQimage = Mat2QImage(tempimage);

        QString widthStr= QString::number((tempimage.size().width));
        QString hightStr= QString::number((tempimage.size().height));
        QImage grayimageQimage = Mat2QImage(tempgrayimage);
        ui->ini_label->setPixmap(QPixmap::fromImage(rgbimageQimage));
        ui->cor_label->setPixmap(QPixmap::fromImage(grayimageQimage));
        ui->lineEdit->setText(widthStr+"X"+hightStr);
        picNum++;
    }
    else
    {
        if(picNum!=0)
        {
            timer1->stop();
            picNum=0;
            QMessageBox::information(NULL, "提示：","图片载入完毕",tr("确定"));

        }
    }
}

void caliForm::findDrawChessCorners()
{
    /*
    Size(5,5) 搜索窗口的一半大小
    Size(-1,-1) 死区的一半尺寸
    TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 20, 0.1)迭代终止条件
    */
    if(picNum<imageMatVec.size())
    {
        isFind = findChessboardCorners(imageMatVec[picNum],boardSize,corner,boardCorner); //所有角点都被找到 说明这幅图像是可行的
        if (isFind == true)
        {
            cornerSubPix(grayimageMatVec[picNum], corner, Size(5,5), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 20, 0.1));
            drawChessboardCorners( imageMatVec[picNum], boardSize, corner,isFind);
            //cornerimageMatvec.push_back(imageMatVec[picNum]);
            QImage rgbQimage = Mat2QImage(imageMatVec[picNum++]);
            ui->Corner_label->setPixmap(QPixmap::fromImage(rgbQimage));
            corners.push_back(corner);
        }
    }
    else
    {
        timer1->stop();
        picNum = 0;
        lastNumForCor= imageMatVec.size();//带角点的图像数量
        QMessageBox::information(this,tr("提示："),"图片角点处理完毕",tr("确定"));

    }

}

void caliForm::caliUndis()
{
    if(picNum<imageMatVec.size())
    {

        undistort(imageMatVec[picNum],undisimage,intrinsic,distortion_coeff);
        undisimageMatvec.push_back(undisimage);
        QImage undisQimage = Mat2QImage(undisimage);
        ui->undis_label->setPixmap(QPixmap::fromImage(undisQimage));

        picNum++;
        }

    else
    {
        timer1->stop();
        flag = true;//表明已经点击了开始标定矫正按钮
        picNum = 0;
        QMessageBox::information(this,tr("提示："),"图片矫正处理完毕",tr("确定"));

    }

}
void caliForm::on_pushButton_clicked()
{
   QList<QString> filename =QFileDialog::getOpenFileNames(this,tr("Open image File"),"/",tr("image Files(*.png *.jpg *.bmp);;All files (*.*)"));
   qDebug()<< filename.size();
   for(auto &file : filename)
   {
       Mat imageMat;
       imageMat=imread(file.toStdString(),1);
       imageHeight = imageMat.size().height;
       imageWidth = imageMat.size().width;
       if(imageMat.empty())
       {
           if(!QMessageBox::warning(this,tr("提示"),tr("图片载入失败,其中一张为空图请检查"),tr("退出")))
                this->close();
       }
       cvtColor(imageMat,grayimage,CV_BGR2GRAY);
       imageMatVec.push_back(imageMat);
       grayimageMatVec.push_back(grayimage);
       ui->lineEdit_2->setText(QString::number(imageMatVec.size()));

   }
   timer1 = new QTimer(this);
   connect(timer1,SIGNAL(timeout()),this,SLOT(showLoadedPics()));
   timer1->start(1000);
}

void caliForm::on_startCalibButton_clicked()
{
    if(imageMatVec.size()==0)
    {
        QMessageBox::warning(this,tr("警告"),tr("请载入标定所需图片"),tr("确定"));

    }
    else if(ui->lineEdit_3->text().isEmpty()||ui->lineEdit_4->text().isEmpty()||ui->lineEdit_5->text().isEmpty())
    {
        if(ui->lineEdit_3->text().isEmpty())
            QMessageBox::warning(this,tr("警告"),tr("请输入横向角点数目"),tr("确定"));
        else if(ui->lineEdit_4->text().isEmpty())

            QMessageBox::warning(this,tr("警告"),tr("请输入纵向角点数目"),tr("确定"));
        else
            QMessageBox::warning(this,tr("警告"),tr("请输入标定黑白格边长(mm)"),tr("确定"));
    }
    else
    {
        boardWidth = ui->lineEdit_3->text().toInt();
        boardHeight = ui->lineEdit_4->text().toInt();
        squareSize = ui->lineEdit_5->text().toInt();
        boardCorner = boardHeight*boardWidth;
        boardSize = Size(boardWidth, boardHeight);

        frameNumber = imageMatVec.size();
        timer1 = new QTimer(this);


        connect(timer1,SIGNAL(timeout()),this,SLOT(findDrawChessCorners()));
        timer1->start(1000);


    }
}

void caliForm::on_caliUndisButton_clicked()
{
    timer1 = new QTimer(this);
    /*设置实际初始参数 根据calibrateCamera来 如果flag = 0 也可以不进行设置*/
    guessCameraParam();
    /*计算实际的校正点的三维坐标*/
    calRealPoint(objRealPoint, boardWidth, boardHeight,imageMatVec.size(), squareSize);
    /*标定摄像头*/
    calibrateCamera(objRealPoint, corners, Size(imageWidth, imageHeight), intrinsic, distortion_coeff, rvecs, tvecs, 0);
    connect(timer1,SIGNAL(timeout()),this,SLOT(caliUndis()));
    timer1->start(1000);

}


void caliForm::on_saveDataButton_clicked()
{
    if(flag == false)
    {
        QMessageBox::warning(this,tr("警告"),tr("尚未标定，请点击标定矫正按钮"),tr("确定"));
    }
    else
       {
        if(ui->comboBox->currentText()=="Left")

         {      FileStorage file(".\\leftData.xml",FileStorage::WRITE);
            file<<"intrinsic"<<intrinsic;
            file<<"distortion_coeff"<<distortion_coeff;
            file<<"rvecs"<<rvecs;
            file<<"tvecs"<<tvecs;
            file<<"imageWidth"<<imageWidth;
            file<<"imageHeight"<<imageHeight;
            file<<"boardWidth"<<boardWidth;
            file<<"boardHeight"<<boardHeight;
            file<<"boardCorner"<<boardCorner;
            file<<"frameNumber"<<frameNumber;
            file<<"squareSize"<<squareSize;
            file<<"boardSize"<<boardSize;
            file<<"corners"<<corners;
            file<<"objRealPoint"<<objRealPoint;
            file<<"imageMatVecL"<<imageMatVec;

            file.release();
            QMessageBox::information(this,tr("提示"),tr("左相机数据已保存"),tr("确定"));
        }
        if(ui->comboBox->currentText()=="Right")

         {      FileStorage file(".\\rightData.xml",FileStorage::WRITE);
                file<<"intrinsic"<<intrinsic;
                file<<"distortion_coeff"<<distortion_coeff;
                file<<"imageWidth"<<imageWidth;
                file<<"imageHeight"<<imageHeight;
                file<<"boardWidth"<<boardWidth;
                file<<"boardHeight"<<boardHeight;
                file<<"boardCorner"<<boardCorner;
                file<<"frameNumber"<<frameNumber;
                file<<"squareSize"<<squareSize;
                file<<"boardSize"<<boardSize;
                file<<"corners"<<corners;
                file<<"objRealPoint"<<objRealPoint;
                file<<"imageMatVecR"<<imageMatVec;
                file.release();
                QMessageBox::information(this,tr("提示"),tr("右相机数据已保存"),tr("确定"));
         }
        }

}

void caliForm::on_saveDataButton_2_clicked()
{
    this->close();
}

void caliForm::on_lastButton_clicked()
{
    if(lastNumForCor<0)
     QMessageBox::information(this,tr("提示"),tr("已经是第一张"),tr("确定"));
    else
        {
            QImage lastQimage = Mat2QImage(imageMatVec[--lastNumForCor]);
            ui->Corner_label->setPixmap(QPixmap::fromImage(lastQimage));
        }
}

void caliForm::on_removeDataButton_clicked()
{
    this->close();
    caliUiNew = new caliForm;//establish new formwindow
    caliUiNew->show();
}

void caliForm::on_DataButton_3_clicked()
{
    this->close();
    caliUiNew = new caliForm;//establish new formwindow
    caliUiNew->show();
}
