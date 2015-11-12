#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    imload = new ImageLoad("/home/cnx/QtProjects/se_project_prototype/", "lena.jpg", CV_LOAD_IMAGE_COLOR);
    context = new WorkingContext(imload->getImage(),
                                 imload->getFgSeeds(),
                                 imload->getBgSeeds());
    algo = new Algorithm();
    Eigen::VectorXd x = algo->solver(context->getA(), context->getB());

    qDebug() << "Applying new labels (colors)";
    cv::Mat image = imload->getImage();//(context->getRows(), context->getCols(), CV_32FC3);
    for(int i = 0; i < context->getRows(); i++)
        for(int j = 0; j < context->getCols(); j++){
            cv::Vec3f color = image.at<cv::Vec3f>(i, j);
            if(x(i*context->getCols()+j)>=(3.+1.)/2.){
                color[0] = 0.; color[1] = 0.; color[2] = 0.;
            }
            image.at<cv::Vec3f>(i, j) = color;
        }
    cv::imshow("image", image);
    cv::waitKey(0);
}

MainWindow::~MainWindow()
{
    delete imload;
    delete ui;
}
