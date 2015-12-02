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
//    t1 = clock();
//    t2 = clock();
//    float diff ((float)t2-(float)t1);
//    float seconds = diff / CLOCKS_PER_SEC;
//    std::cout<<seconds<< "s" <<std::endl;
    ui->process->setEnabled(false);
    ui->algstart->setEnabled(false);
    ui->ground->setEnabled(false);
    ui->horizontalSlider->setEnabled(false);
    ui->saveButton->setEnabled(false);
}

MainWindow::~MainWindow()
{
//    delete imageLabel;
//    delete context;
    delete ui;
}

void MainWindow::on_Button_open_clicked()
{
    QStringList mimeTypeFilters;
    foreach (const QByteArray &mimeTypeName, QImageReader::supportedMimeTypes())
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    QFileDialog dialog(this, tr("Open File"), picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.first());
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
    cvimage = cv::imread(fileName.toStdString());
    cvimage.convertTo(cvimage, CV_32F);
    timage = image;
    rimage = image;
    ui->Button_open->setEnabled(true);
    ui->process->setEnabled(true);
    ui->horizontalSlider->setEnabled(true);
    // set the image in the label
    imageLabel->setPixmap(QPixmap::fromImage(timage));
    imageLabel->adjustSize();
    setWindowFilePath(fileName);
    //showImage(cvimage);
    return true;
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    if (!imageLabel->pixmap()) {
        qDebug() << "Image label is empty!";
        return;
    }
    double factor;
    factor = value*0.2;
    imageLabel->resize(factor * imageLabel->pixmap()->size());
    ui->scrollArea->horizontalScrollBar()->setValue(int(factor * ui->scrollArea->horizontalScrollBar()->value() + ((factor - 1) * ui->scrollArea->horizontalScrollBar()->pageStep()/2)));
    ui->scrollArea->verticalScrollBar()->setValue(int(factor * ui->scrollArea->verticalScrollBar()->value() + ((factor - 1) * ui->scrollArea->verticalScrollBar()->pageStep()/2)));
}


void MainWindow::on_algstart_clicked()
{
    context = new WorkingContext(cvimage, fseeds, bseeds, 0.1, 0.0001, 3.0, 1.0);
    cv::Mat __image = context->getSegmentation();
    showImage(__image);
}

void MainWindow::showImage(const cv::Mat &pimage)
{
    cv::Mat image = pimage.clone();
    image.convertTo(image, CV_8U);
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
}


void MainWindow::on_process_clicked()
{
    if(buttonpro==true)
    {
        ui->Button_open->setEnabled(false);
        ui->horizontalSlider->setEnabled(false);
        ui->ground->setEnabled(true);
        ui->process->setText("Reset");
        ui->horizontalSlider->setValue(5);
        buttonpro= false;
        paint = true;
    }
    else
    {
        ui->Button_open->setEnabled(true);
        ui->process->setEnabled(true);
        ui->horizontalSlider->setEnabled(true);
        ui->algstart->setEnabled(false);
        ui->ground->setEnabled(false);
        ui->saveButton->setEnabled(false);
        ui->process->setText("Process");
        timage = rimage;
        imageLabel->setPixmap(QPixmap::fromImage(timage));
        ui->ground->setText("Foreground");
        select = true;
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

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    xpoint = event->x();
    ypoint = event->y();
    xpoint = xpoint - ui->scrollArea->geometry().x();
    ypoint = ypoint - ui->scrollArea->geometry().y();
    if((xpoint>0)&&(xpoint<imageLabel->geometry().width())&&(ypoint>0)&&(ypoint<imageLabel->geometry().height()))
        if(paint == true)
        {
            QPainter painter(&timage);
            if(select == true)
            {
                paintpen.setColor(Qt::red);
                paintpen.setWidth(1);
            }
            else
            {
                paintpen.setColor(Qt::green);
                paintpen.setWidth(1);
            }
            QPoint p1;
            p1.setX(xpoint);
            p1.setY(ypoint);
            painter.setPen(paintpen);
            painter.drawPoint(p1);
            imageLabel->setPixmap(QPixmap::fromImage(timage));
            qDebug() << xpoint << ypoint;
            if(select)
                fseeds.insert(std::make_pair(ypoint, xpoint));
            else
                bseeds.insert(std::make_pair(ypoint, xpoint));
        }
}

