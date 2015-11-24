#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    imageLabel = new QLabel;
    imageLabel->setBackgroundRole(QPalette::Dark);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);
    ui->scrollArea->setBackgroundRole(QPalette::Dark);
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->scrollArea->setWidget(imageLabel);

    imload = new ImageLoad("/Users/Songyou/Documents/VIBOT/UB/Software Engineering/Project/s-image-s/images/", "lena.jpg", CV_LOAD_IMAGE_COLOR);
    context = new WorkingContext(imload->getImage(),
                                 imload->getFgSeeds(),
                                 imload->getBgSeeds());
    algo = new Algorithm();
    Eigen::VectorXd x = algo->solver(context->getA(), context->getB());

    qDebug() << "Applying new labels (colors)";
    cv::Mat image = imload->getImage();//(context->getRows(), context->getCols(), CV_32FC3);
    cv::Mat image_c(image.rows, image.cols, image.type());
    cv::Vec3f noir;
    noir[0] = 0.; noir[1] = 0.; noir[2] = 0.;
    for(int i = 0; i < context->getRows(); i++)
        for(int j = 0; j < context->getCols(); j++){
            if(x(i*context->getCols()+j)<(3.+1.)/2.){
                image_c.at<cv::Vec3f>(i,j) = image.at<cv::Vec3f>(i,j);

            } else {
                image_c.at<cv::Vec3f>(i, j) = noir;
            }
        }
    showImage(image_c);
    /*
     * TODO: for gui:
     * zoom in, zoom out, fit to window, or horizontal slider,
     * open file, save, close
     * seeds palette
    */
}

void MainWindow::scaleImage(double factor)
{
    if (!imageLabel->pixmap()) {
        qDebug() << "Image label is empty!";
        return;
    }
    imageLabel->resize(factor * imageLabel->pixmap()->size());
    adjustScrollBar(ui->scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(ui->scrollArea->verticalScrollBar(), factor);
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}

void MainWindow::showImage(const cv::Mat &pimage)
{
    cv::Mat image = pimage.clone();
    image.convertTo(image, CV_8SC3);
    QImage qimage;
    //this is from Yohan's code
    if( image.channels() == 3)
        qimage = QImage((const unsigned char*)(image.data),
                              image.cols, image.rows,
                              image.step, QImage::Format_RGB888).rgbSwapped();
    else if( image.channels() == 1)
        qimage = QImage((const unsigned char*)(image.data),
                              image.cols, image.rows,
                              image.step, QImage::Format_Indexed8);
    else
    {
        qDebug()<<"Error: wrong image format";
    }
    imageLabel->setPixmap(QPixmap::fromImage(qimage));
    scaleImage(1.0);
}

MainWindow::~MainWindow()
{
    delete imload;
    delete context;
    delete algo;
    delete ui;
}
