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
    buttonpro = true;
    select = true;
    paint = false;

    //imload = new ImageLoad("../seoop/images/", "lena.jpg", CV_LOAD_IMAGE_COLOR);
//    imload = new ImageLoad("/Users/Songyou/Desktop/Samples/", "lena.jpg", CV_LOAD_IMAGE_COLOR);

    ui->process->setEnabled(false);
    ui->algstart->setEnabled(false);
    ui->ground->setEnabled(false);
    ui->horizontalSlider->setEnabled(false);
    ui->saveButton->setEnabled(false);
    /*
     * TODO: for gui:
     * zoom in, zoom out, fit to window, or horizontal slider,
     * open file, save, close
     * seeds palette
    */
}

MainWindow::~MainWindow()
{
//    delete imload;
//    delete context;
//    delete algo;
    delete ui;
}

void MainWindow::on_Button_open_clicked()
{
    QStringList mimeTypeFilters;
    foreach (const QByteArray &mimeTypeName, QImageReader::supportedMimeTypes())
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    QFileDialog dialog(this, tr("Open File"),
                       picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.first());
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");

    while (dialog.exec() == QDialog::Accepted && !loadImg(dialog.selectedFiles().first())) {}
}

bool MainWindow::loadImg(const QString &fileName)
{
    QImage image(fileName);
    if (image.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1.").arg(QDir::toNativeSeparators(fileName)));
        setWindowFilePath(QString());
        imageLabel->setPixmap(QPixmap());
        ui->Button_open->setEnabled(true);
        return false;
    }
    ui->Button_open->setEnabled(true);
    ui->process->setEnabled(true);
    ui->horizontalSlider->setEnabled(true);
    // set the image in the label
    imageLabel->setPixmap(QPixmap::fromImage(image));
    imageLabel->adjustSize();
    setWindowFilePath(fileName);
    return true;
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    if (!imageLabel->pixmap()) {
        qDebug() << "Image label is empty!";
        return;
    }
    double factor;
    if(value>5)
    {
    factor = value*0.2;
    imageLabel->resize(factor * imageLabel->pixmap()->size());
    }
    else
    {
    factor = value*0.2;
    imageLabel->resize(factor * imageLabel->pixmap()->size());
    }
    ui->scrollArea->horizontalScrollBar()->setValue(int(factor * ui->scrollArea->horizontalScrollBar()->value() + ((factor - 1) * ui->scrollArea->horizontalScrollBar()->pageStep()/2)));
    ui->scrollArea->verticalScrollBar()->setValue(int(factor * ui->scrollArea->verticalScrollBar()->value() + ((factor - 1) * ui->scrollArea->verticalScrollBar()->pageStep()/2)));
}


void MainWindow::on_algstart_clicked()
{
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
}

void MainWindow::showImage(const cv::Mat &pimage)
{
    cv::Mat image = pimage.clone();
    image.convertTo(image, CV_8SC3);
    QImage qimage;
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
//    scaleImage(5.0);
}


void MainWindow::on_process_clicked()
{
    if(buttonpro==true)
    {
        ui->Button_open->setEnabled(false);
        ui->horizontalSlider->setEnabled(false);
        ui->ground->setEnabled(true);
        ui->process->setText("Reset");
        buttonpro= false;
        paint = true;
    }
    else
    {
        ui->Button_open->setEnabled(true);
        ui->process->setEnabled(false);
        ui->horizontalSlider->setEnabled(false);
        ui->algstart->setEnabled(false);
        ui->ground->setEnabled(false);
        ui->saveButton->setEnabled(false);
        ui->process->setText("Process");
        buttonpro = true;
        paint = false;
    }
}

void MainWindow::on_ground_clicked()
{
    ui->algstart->setEnabled(true);
    if(select==true)
    {
        ui->Button_open->setEnabled(false);
        ui->horizontalSlider->setEnabled(false);
        ui->ground->setText("Background");

//        QPainter painter(&tempimg);
//        QPen paintpen(Qt::red);
//        paintpen.setWidth(10);
//        QPoint p1;
//        p1.setX(mFirstX);
//        p1.setY(mFirstY);
//        painter.setPen(paintpen);
//        painter.drawPoint(p1);
//        imageLabel->setPixmap(QPixmap::fromImage(tempimg));

        select = false;
    }
    else
    {
        ui->Button_open->setEnabled(false);
        ui->horizontalSlider->setEnabled(false);
        ui->ground->setText("Foreground");
        select = true;
    }
}

//void MainWindow::mouseMoveEvent( QMouseEvent* event )
//{

//    int x = event->x();
//    int y = event->y();
//    qDebug()<< x << y;
//}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    xpoint = event->x();
    ypoint = event->y();
    qDebug()<<xpoint<<ypoint;
    if(paint == true)
    {
    QImage image(imageLabel->pixmap()->toImage());
    QPainter painter(&image);
    if(select == true)
    {
    paintpen.setColor(Qt::red);
    paintpen.setWidth(4);
    }
    else
    {
    paintpen.setColor(Qt::green);
    paintpen.setWidth(4);
    }
    QPoint p1;
    p1.setX(xpoint);
    p1.setY(ypoint);
    painter.setPen(paintpen);
    painter.drawPoint(p1);
    imageLabel->setPixmap(QPixmap::fromImage(image));
    }
}

