#include "camerawindow.h"
#include"caliform.h"
#include"doublecali.h"
#include "ui_camerawindow.h"
#include"processui.h"
//using namespace  cv;
cameraWindow::cameraWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::cameraWindow)
{
    ui->setupUi(this);
    ui->label->setScaledContents(true);//fit video to lable area
}

cameraWindow::~cameraWindow()
{
    delete ui;
}
QImage MatToImage(const cv::Mat& mat)
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

void cameraWindow::nextFrame()
{
    capture >> frame;
    switch (videoFlag) {
    case false:
        if (!frame.empty())
        {
            image = MatToImage(frame);
            ui->label->setPixmap(QPixmap::fromImage(image));
            //this->update();
        }
        break;
    case true:
        if (!frame.empty())
        {
            image = MatToImage(frame);
            ui->label->setPixmap(QPixmap::fromImage(image));
             writer.write(frame);
        }
        break;
    default:
        break;

    }


}




void cameraWindow::on_Camera_On_Button_clicked()
{

    if (capture.isOpened())
            capture.release();     //decide if capture is already opened; if so,close it
    capture.open(0);           //open the default camera
    if (capture.isOpened())
        {
           //rate= capture.get(CV_CAP_PROP_FPS);
           capture >> frame;
           if (!frame.empty())
            {
                image =  MatToImage(frame);
                ui->label->setPixmap(QPixmap::fromImage(image));
                timer = new QTimer(this);
                timer->setInterval(1000/30);   //set timer match with FPS
                connect(timer, SIGNAL(timeout()), this, SLOT(nextFrame()));
                timer->start();
            }
        }
}

void cameraWindow::on_video_On_Button_clicked()
{
    if (capture.isOpened())
           capture.release();  //decide if capture is already opened; if so,close it
       QString filename =QFileDialog::getOpenFileName(this,tr("Open Video File"),".",tr("Video Files(*.avi *.mp4 *.flv *.mkv)"));
       capture.open(filename.toLocal8Bit().data());
       if (capture.isOpened())
       {
           rate= capture.get(CV_CAP_PROP_FPS);
           capture >> frame;
           if (!frame.empty())
           {

               image =  MatToImage(frame);
               ui->label->setPixmap(QPixmap::fromImage(image));
               timer = new QTimer(this);
               timer->setInterval(1000/rate);   //set timer match with FPS
               connect(timer, SIGNAL(timeout()), this, SLOT(nextFrame()));
               timer->start();
           }
       }
}

void cameraWindow::on_Process_Button_3_clicked()
{
    cv::Mat cannyImg ;
    cv::Canny(frame, cannyImg, 0, 30, 3);
    QImage cannyimage =  MatToImage(cannyImg);
    ui->label_3->setPixmap(QPixmap::fromImage(cannyimage));
//    cv::namedWindow("Canny");
//    cv::imshow("Canny", cannyImg);
}

void cameraWindow::on_StartRec_Button_clicked()
{
    if (capture.isOpened())
            capture.release();     //decide if capture is already opened; if so,close it
    capture.open(0);           //open the default camera


    videoFlag = true;
    if(capture.isOpened())
    {

        capture>>frame;
        image =  MatToImage(frame);
        timer = new QTimer(this);
        writer.open("./myrec.avi",VideoWriter::fourcc('P','I','M','1'), /*capture.get(CV_CAP_PROP_FPS)*/25, cv::Size(frame.cols, frame.rows),true);
        connect(timer,SIGNAL(timeout()),this,SLOT(nextFrame()));
        timer->start(1000/25);
    }
}

void cameraWindow::on_EndRec_Button_clicked()
{
    videoFlag = false;
    timer->stop();
    capture.release();
    writer.release();
}

void cameraWindow::on_CloseCamera_Button_clicked()
{
    timer->stop();
    capture.release();
}

void cameraWindow::on_EndRec_Button_2_clicked()
{
    scrPicMat = frame;    
    scrPic =  MatToImage(frame);
    ui->label_2->setPixmap(QPixmap::fromImage(scrPic));
    ui->SaveScrpic_Button->setEnabled(true);
}

void cameraWindow::on_SaveScrpic_Button_clicked()
{
 string filename = "D:\\QT proj\\cameraDemo\\SrcPic\\" + to_string(picNum++)+".jpg";
 imwrite(filename,scrPicMat);
 QMessageBox::about(NULL, "提示：", "图片已保存");
  ui->SaveScrpic_Button->setDisabled(true);

}

void cameraWindow::on_Quit_Button_clicked()
{
    if (!(QMessageBox::information(this,tr("提示"),tr("Do you really want to log out ?"),tr("Yes"),tr("No"))))
             {
                caliUi->close();
                proUi->close();
                  this->close();
              }
}

void cameraWindow::on_Cali_Button_clicked()
{
    int rb = QMessageBox::question(this, tr("类型选择"), tr("双目摄像头或单目摄像头?"), tr("单目"),tr("双目"));
    if(rb == 0)
    {
        caliUi = new caliForm;//establish new formwindow
        caliUi->show();
       // this->hide();

    }
    if(rb == 1)
    {
        doublecali = new DoubleCali;//establish new formwindow
        doublecali->show();
       // this->hide();

    }
}

void cameraWindow::on_Cali_Button_2_clicked()
{
    doublecali = new DoubleCali;//establish new formwindow
    doublecali->show();
}

void cameraWindow::on_process_Button_clicked()
{
    proUi = new processUi;//establish new formwindow

    proUi->show();
}

void cameraWindow::on_process_Button_2_clicked()
{
    proUi = new processUi;//establish new formwindow
    proUi->show();
}
