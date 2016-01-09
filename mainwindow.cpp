#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    imageLabel = new LabelDraw;
    imageLabel->setBackgroundRole(QPalette::Dark);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);
    progressBar = new QProgressBar;
    progressBar->setValue(0);
    progressBar->setVisible(false);
    ui->statusBar->addWidget(progressBar);
    ui->ground->addItem(QString("Background"));
    ui->ground->addItem(QString("Foreground"));
    connect(imageLabel, SIGNAL(mousePressed()), this, SLOT(mousePressed()));
    connect(imageLabel, SIGNAL(mouseMoved()), this, SLOT(mouseMoved()));
    connect(ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(on_Button_open_clicked()));
    connect(ui->actionSave, SIGNAL(triggered(bool)), this, SLOT(on_saveButton_clicked()));
    ui->scrollArea->setBackgroundRole(QPalette::Dark);
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->scrollArea->setWidget(imageLabel);
    buttonpro = true;
    select = 0;
    paint = false;
    ui->process->setEnabled(false);
    ui->algstart->setEnabled(false);
    ui->ground->setEnabled(false);
    ui->horizontalSlider->setEnabled(false);
    ui->saveButton->setEnabled(false);
    thread = new QThread();
}

MainWindow::~MainWindow()
{
    delete imageLabel;
    delete worker;
    delete thread;
    delete ui;
}

void MainWindow::on_Button_open_clicked()
{
    QString maxheight = ui->uiMaxHeight->text();
    QString maxwidth = ui->uiMaxWidth->text();
    bool check1,check2;
    maxheight.toUShort(&check1);
    maxwidth.toUShort(&check2);
    if(!(check1&check2))
    {
        QMessageBox::information(this, "Port Value","Enter Valid Range Vaule");
    }
    else
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
}

bool MainWindow::loadImg(const QString &fileName)
{
    QImage image(fileName);
    int height = ui->uiMaxHeight->text().toInt();
    int width = ui->uiMaxWidth->text().toInt();
    if (image.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1.").arg(QDir::toNativeSeparators(fileName)));
        setWindowFilePath(QString());
        imageLabel->setPixmap(QPixmap());
        ui->Button_open->setEnabled(true);
        return false;
    }
    cvimage = cv::imread(fileName.toStdString());
    int nwidth, nheight;
    bool changed = false;
    if(image.width() > width){
        image = image.scaledToWidth(width);
        nwidth = image.width();
        nheight = image.height();
        changed = true;
    }
    if(image.height() > height){
        image = image.scaledToHeight(height);
        nwidth = image.width();
        nheight = image.height();
        changed = true;
    }
    if(changed)
        cv::resize(cvimage, cvimage, cv::Size(nwidth, nheight));
    qDebug() << cvimage.cols << cvimage.rows;
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
    ui->statusBar->showMessage(QString("Image size: %1x%2").arg(cvimage.cols).arg(cvimage.rows));
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
    if(bseeds.empty()){
        QMessageBox::warning(this, "Seeds not selected!", "Please select background seeds.");
        return;
    }
    if(fseeds.empty()){
        QMessageBox::warning(this, "Seeds not selected!", "Please select foreground seeds.");
        return;
    }
    ui->algstart->setEnabled(false);
    ui->process->setEnabled(false);
    ui->saveButton->setEnabled(false);
    worker = new WorkerThread(cvimage, fseeds, bseeds, 0.1, 0.0001, 3.0, 1.0);
    worker->moveToThread(thread);
    connect(worker, SIGNAL(progressEvent(int,QString)), this, SLOT(progressUpdate(int,QString)));
    connect(worker, SIGNAL(error(QString)), this, SLOT(errorHandler(QString)));
    connect(thread, SIGNAL(started()), worker, SLOT(process()));
    connect(worker, SIGNAL(finished(QImage, QString)), this, SLOT(showImage(QImage, QString)));
    connect(worker, SIGNAL(finished(QImage, QString)), thread, SLOT(quit()));
    connect(worker, SIGNAL(finished(QImage, QString)), worker, SLOT(deleteLater()));
    thread->start();
}

void MainWindow::showImage(const QImage &pimage, const QString &message)
{
    imageLabel->setPixmap(QPixmap::fromImage(pimage));
    ui->statusBar->showMessage(message);
}

void MainWindow::progressUpdate(int value, QString text)
{
    progressBar->setFormat(QString("%1: %p%").arg(text));
    progressBar->setVisible(true);
    progressBar->setValue(value);
    if (value==progressBar->maximum()) {
        progressBar->setVisible(false);
        ui->process->setEnabled(true);
        ui->saveButton->setEnabled(true);
        ui->actionSave->setEnabled(true);
        ui->horizontalSlider->setEnabled(true);
    }
}


void MainWindow::on_process_clicked()
{
    if(buttonpro==true)
    {
        ui->Button_open->setEnabled(false);
        ui->actionOpen->setEnabled(false);
        ui->uiMaxHeight->setEnabled(false);
        ui->uiMaxWidth->setEnabled(false);
        ui->horizontalSlider->setEnabled(false);
        ui->ground->setEnabled(true);
        ui->process->setText("Reset");
        ui->algstart->setEnabled(true);
        ui->saveButton->setEnabled(true);
        ui->actionSave->setEnabled(true);
        ui->horizontalSlider->setValue(5);
        select = ui->ground->currentIndex();
        qDebug() << "select on process:" << select;
        buttonpro= false;
        paint = true;
    }
    else
    {
        fseeds.clear();
        bseeds.clear();
        ui->Button_open->setEnabled(true);
        ui->actionOpen->setEnabled(true);
        ui->uiMaxHeight->setEnabled(true);
        ui->uiMaxWidth->setEnabled(true);
        ui->process->setEnabled(true);
        ui->horizontalSlider->setEnabled(true);
        ui->algstart->setEnabled(false);
        ui->ground->setEnabled(false);
        ui->saveButton->setEnabled(false);
        ui->actionSave->setEnabled(false);
        ui->process->setText("Process");
        timage = rimage;
        imageLabel->setPixmap(QPixmap::fromImage(timage));
        buttonpro = true;
        paint = false;
    }
}

void MainWindow::mousePressed()
{
    xpointStart = imageLabel->getX();
    ypointStart = imageLabel->getY();
    if((xpointStart>0)&&(xpointStart<imageLabel->geometry().width())&&(ypointStart>0)&&(ypointStart<imageLabel->geometry().height()))
        if(paint == true)
        {
            if(select==1)
                fseeds.insert(std::make_pair(ypointStart, xpointStart));
            else
                bseeds.insert(std::make_pair(ypointStart, xpointStart));
        }
}

void MainWindow::mouseMoved()
{
    xpointEnd = imageLabel->getX();
    ypointEnd = imageLabel->getY();
    if((xpointEnd>0)&&(xpointEnd<imageLabel->geometry().width())&&(ypointEnd>0)&&(ypointEnd<imageLabel->geometry().height()))
        if(paint == true)
        {
            QPainter painter(&timage);
            if(select == 0)
            {
                paintpen.setColor(Qt::red);
                paintpen.setWidth(4);
            }
            else if(select == 1)
            {
                paintpen.setColor(Qt::green);
                paintpen.setWidth(4);
            }
            else if(select == 2)
            {
                paintpen.setColor(Qt::yellow);
                paintpen.setWidth(4);
            }
            else if(select == 3)
            {
                paintpen.setColor(Qt::blue);
                paintpen.setWidth(4);
            }
            else if(select == 4)
            {
                paintpen.setColor(Qt::black);
                paintpen.setWidth(4);
            }
            else if(select == 5)
            {
                paintpen.setColor(Qt::white);
                paintpen.setWidth(4);
            }
            painter.setPen(paintpen);
            painter.drawLine(xpointStart, ypointStart, xpointEnd, ypointEnd);
            xpointStart = xpointEnd;
            ypointStart = ypointEnd;
            imageLabel->setPixmap(QPixmap::fromImage(timage));
            if(select == 1)
                fseeds.insert(std::make_pair(ypointEnd, xpointEnd));
            else if(select == 0)
                bseeds.insert(std::make_pair(ypointEnd, xpointEnd));
//            else if(select == 2)
//                bseeds.insert(std::make_pair(ypointEnd, xpointEnd));
//            else if(select == 3)
//                bseeds.insert(std::make_pair(ypointEnd, xpointEnd));
//            else if(select == 4)
//                bseeds.insert(std::make_pair(ypointEnd, xpointEnd));
//            else if(select == 5)
//                bseeds.insert(std::make_pair(ypointEnd, xpointEnd));

        }
}

void MainWindow::errorHandler(QString err)
{
    ui->statusBar->showMessage(err);
}


void MainWindow::on_ground_currentIndexChanged(int index)
{
    select = index;
    qDebug() << "selected index:" << select;
}

void MainWindow::on_uiFgrNum_currentIndexChanged(int index)
{
    ui->ground->clear();
    ui->ground->addItem(QString("Background"));
    ui->ground->addItem(QString("Foreground"));
    for(int i = 0; i < index; i++)
    {
//        QToolButton *btn = new QToolButton();
//        btn->setText(QString("%1").arg(i));
//        ui->colorContainer->addWidget(btn);
        ui->ground->addItem(QString("Foreground-%1").arg(i+2));
    }
}

void MainWindow::on_uiMaxWidth_editingFinished()
{
    QString maxvalue = ui->uiMaxWidth->text();
    bool check;
    maxvalue.toUShort(&check);
    if(!check)
    {
        QMessageBox::information(this, "Port Value","Enter Valid Range Vaule");
    }
}

void MainWindow::on_uiMaxHeight_editingFinished()
{
    QString maxvalue = ui->uiMaxHeight->text();
    bool check;
    maxvalue.toUShort(&check);
    if(!check)
    {
        QMessageBox::information(this, "Port Value","Enter Valid Range Vaule");
    }
}

/**
 * @brief MainWindow::on_saveButton_clicked
 * @cite: http://creative-punch.net/2014/02/opening-displaying-saving-images-qt
 */

void MainWindow::on_saveButton_clicked()
{
    QImage image = imageLabel->pixmap()->toImage();
    if(!image.isNull()){
        QString imagePath = QFileDialog::getSaveFileName(
                        this,
                        tr("Save File"),
                        "",
                        tr("JPEG (*.jpg *.jpeg);;PNG (*.png)" )
                        );
        if(!imagePath.isEmpty())
            image.save(imagePath);
    }
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit(0);
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this, "About", "This application is used to segment images using seed based method described in paper \"Laplacian Coordinates for Seeded Image Segmentation\" by Casaca, W. ; ICMC, Univ. of Sao Paulo, São Carlos, Brazil ; Nonato, L.G. ; Taubin, G.\n\nImplemented by Kushibar K., Peng S., Veilu Muthu V.");
}

/**
 * @brief MainWindow::on_actionHelp_triggered
 *
 */
void MainWindow::on_actionHelp_triggered()
{
    ui_help.show();
}
