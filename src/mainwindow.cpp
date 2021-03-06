#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Starting the Constructor of Mainwindow class.
    // Initialize the mouse action parameters.
    buttonpro = true;
    select = 0;
    paint = false;

    // Creating the Label and progressbar for mainwindow.
    imageLabel = new LabelDraw;
    imageLabel->setBackgroundRole(QPalette::Dark);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);
    progressBar = new QProgressBar;
    progressBar->setValue(0);
    progressBar->setVisible(false);

    // Initializing the statusbar dropdown box and scrollarea part.
    ui->statusBar->addWidget(progressBar);
    ui->ground->addItem(QString("Background"));
    ui->ground->addItem(QString("Foreground"));
    ui->scrollArea->setBackgroundRole(QPalette::Dark);
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->scrollArea->setWidget(imageLabel);

    // Connecting the Mouse action and the action section.
    connect(imageLabel, SIGNAL(mousePressed()), this, SLOT(mousePressed()));
    connect(imageLabel, SIGNAL(mouseMoved()), this, SLOT(mouseMoved()));
    connect(ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(on_Button_open_clicked()));
    connect(ui->actionSave, SIGNAL(triggered(bool)), this, SLOT(on_saveButton_clicked()));

    // Disable for the preset start.
    ui->process->setEnabled(false);
    ui->algstart->setEnabled(false);
    ui->ground->setEnabled(false);
    ui->horizontalSlider->setEnabled(false);
    ui->saveButton->setEnabled(false);
    // Create the Thread for the algorithm process.
    ui->actionSave->setEnabled(false);
    thread = new QThread();
    worker = NULL;
}

MainWindow::~MainWindow()
{
    delete thread;
    delete ui;
}

//%%%%%%%%%%%%%%%%% LOAD IMAGE SECTION %%%%%%%%%%%%
// Check for the valid entry in the width text editor
void MainWindow::on_uiMaxWidth_editingFinished()
{
    QString maxvalue = ui->uiMaxWidth->text();
    bool check;
    maxvalue.toUShort(&check);
    if(!check)
    {
        QMessageBox::information(this, "Error message","Enter Valid Range Vaule");
    }
}
// Check for the valid entry in the height text editor
void MainWindow::on_uiMaxHeight_editingFinished()
{
    QString maxvalue = ui->uiMaxHeight->text();
    bool check;
    maxvalue.toUShort(&check);
    if(!check)
    {
        QMessageBox::information(this, "Error message","Enter Valid Range Vaule");
    }
}

// Pushbutton for loading the image
void MainWindow::on_Button_open_clicked()
{
    // Grab the Height and Width for the text editor/
    QString maxheight = ui->uiMaxHeight->text();
    QString maxwidth = ui->uiMaxWidth->text();

    // Check for the valid entry is numbers.
    bool check1,check2;
    maxheight.toUShort(&check1);
    maxwidth.toUShort(&check2);
    if(!(check1&check2))
    {
        QMessageBox::information(this, "Error message","Enter Valid Range Vaule");
    }
    else
    {
        // Open the Dialog box and select the required image for the processing.
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

// Resize the Image Before displaying in the Label.
bool MainWindow::loadImg(const QString &fileName)
{
    // Load QImage to display and check the image is valid.
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

    buttonpro = true;
    on_process_clicked();

    // Initialize the cv Image for the segmentation.
    cvimage = cv::imread(fileName.toStdString());

    // Set the size limit for height and width.
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
    // When there is a new image is loaded change the size of the image.
    if(changed)
        cv::resize(cvimage, cvimage, cv::Size(nwidth, nheight));
    // Convert the image formet for the process
    cvimage.convertTo(cvimage, CV_32F);
    // Reserve the Image for reset.
    timage = image;
    rimage = image;
    // Enable the buttons for the next porcess
    ui->Button_open->setEnabled(true);
    ui->process->setEnabled(true);
    ui->horizontalSlider->setEnabled(true);
    // set the image in the label
    imageLabel->setPixmap(QPixmap::fromImage(timage));
    imageLabel->adjustSize();
    setWindowFilePath(fileName);
    // Show the status of image size
    ui->statusBar->showMessage(QString("Image size: %1x%2").arg(cvimage.cols).arg(cvimage.rows));
    return true;
}

//%%%%%%%%%%%%% SELECTION SECTION %%%%%%%%%%%%%%%%%
// setting the slider ratio property with the scale factor
void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    if (!imageLabel->pixmap()) {
        qDebug() << "Image label is empty!";
        return;
    }
    double factor;
    factor = value*0.2;
    imageLabel->resize(factor * imageLabel->pixmap()->size());
    // Setting the horizontal and vertical scrollbar factor ratio.
    ui->scrollArea->horizontalScrollBar()->setValue(int(factor * ui->scrollArea->horizontalScrollBar()->value() + ((factor - 1) * ui->scrollArea->horizontalScrollBar()->pageStep()/2)));
    ui->scrollArea->verticalScrollBar()->setValue(int(factor * ui->scrollArea->verticalScrollBar()->value() + ((factor - 1) * ui->scrollArea->verticalScrollBar()->pageStep()/2)));
}

// Creating a toggle button with the pushbutton for enabling and disabling.
void MainWindow::on_process_clicked()
{
    if(buttonpro==true)
    {
        // When the process button is clicked
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
        // When the Reset button is clicked.
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

// Selecting the mouse action for reading the mouse position
void MainWindow::mousePressed()
{
    //Read x & y coordinated from the screen.
    xpointStart = imageLabel->getX();
    ypointStart = imageLabel->getY();
    if((xpointStart>0)&&(xpointStart<imageLabel->geometry().width())&&(ypointStart>0)&&(ypointStart<imageLabel->geometry().height()))
        // Making a point of selection between the seeds.
        if(paint == true)
        {
            if(select==1)
                fseeds.insert(std::make_pair(ypointStart, xpointStart));
            else
                bseeds.insert(std::make_pair(ypointStart, xpointStart));
        }
}

// Defining various color for each seeds.
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
            // Reading the point from the mouse and label the point color on the displayed image.
            painter.setPen(paintpen);
            painter.drawLine(xpointStart, ypointStart, xpointEnd, ypointEnd);
            xpointStart = xpointEnd;
            ypointStart = ypointEnd;
            imageLabel->setPixmap(QPixmap::fromImage(timage));
            // Reading the value to the seeds.
            if(select == 0)
                bseeds.insert(std::make_pair(ypointEnd, xpointEnd));
            else if(select == 1)
                fseeds.insert(std::make_pair(ypointEnd, xpointEnd));
        }
}

// Selecting the index for the different ground layer.
void MainWindow::on_ground_currentIndexChanged(int index)
{
    select = index;
}


//%%%%%%%%%%%%%% PROCESS SECTION %%%%%%%%%%%%%%%%%%%%
// Check for the valid entry in the beta text editor
void MainWindow::on_uiBetaValue_editingFinished()
{
    QString betavalue = ui->uiBetaValue->text();
    bool check;
    betavalue.toDouble(&check);
    if(!check)
    {
        QMessageBox::information(this, "Error message","Enter Valid Integer");
    }
}
// Starting the algorithm process
void MainWindow::on_algstart_clicked()
{
    QString betavalue = ui->uiBetaValue->text();
    bool check;
    betavalue.toDouble(&check);
    if(!check)
    {
        QMessageBox::information(this, "Error message","Enter Valid Integer");
    }
    else
    {
        double beta = betavalue.toDouble();
    if(bseeds.empty()){
        QMessageBox::warning(this, "Seeds not selected!", "Please select background seeds.");
        return;
    }
    if(fseeds.empty()){
        QMessageBox::warning(this, "Seeds not selected!", "Please select foreground seeds.");
        return;
    }
    // Disabling other setting when the algorithm is in run.
    ui->algstart->setEnabled(false);
    ui->process->setEnabled(false);
    ui->saveButton->setEnabled(false);
    // Creating the worker thread for process.
    worker = new WorkerThread(cvimage, fseeds, bseeds, 0.1, beta, 3.0, 1.0); //sigma,beta,foreground,background
    worker->moveToThread(thread);
    // Reference:
    // How to use qthreads in a proper way
    // https://mayaposch.wordpress.com/2011/11/01/how-to-really-truly-use-qthreads-the-full-explanation/

    // when progressEvent emitted, update progress bar in GUI
    connect(worker, SIGNAL(progressEvent(int,QString)), this, SLOT(progressUpdate(int,QString)));
    // when thread encountered an error, call errorHandler in mainwindow
    connect(worker, SIGNAL(error(QString)), this, SLOT(errorHandler(QString)));
    // when thread is started, call process in worker
    connect(thread, SIGNAL(started()), worker, SLOT(process()));
    // when worker finished, show the result in GUI
    connect(worker, SIGNAL(finished(QImage, QString)), this, SLOT(showImage(QImage, QString)));
    // when worker finished, quit thread
    connect(worker, SIGNAL(finished(QImage, QString)), thread, SLOT(quit()));
    // when worker finished, delete the instance of worker
    connect(worker, SIGNAL(finished(QImage, QString)), worker, SLOT(deleteLater()));
    thread->start();
    fseeds.clear();
    bseeds.clear();
    }
}

//Saving the label image to the directary.
//Reference: http://creative-punch.net/2014/02/opening-displaying-saving-images-qt
void MainWindow::on_saveButton_clicked()
{
    QImage image = imageLabel->pixmap()->toImage();
    if(!image.isNull()){
        QString imagePath = QFileDialog::getSaveFileName(this,tr("Save File"),"",tr("JPEG (*.jpg *.jpeg);;PNG (*.png)" ));
        if(!imagePath.isEmpty())
            image.save(imagePath);
    }
}

// Trigger Quit button
void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit(0);
}

//%%%%%%%%%%% DISPLAY SECTION %%%%%%%%%%%%%%%%
// Displaying the processed image in the Label
void MainWindow::showImage(const QImage &pimage, const QString &message)
{
    imageLabel->setPixmap(QPixmap::fromImage(pimage));
    ui->statusBar->showMessage(message);
}

// Setting the process update information in progressbar
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
        ui->Button_open->setEnabled(true);
        ui->actionOpen->setEnabled(true);
        paint = false;
    }
}

// Displaying the error message
void MainWindow::errorHandler(QString err)
{
    ui->statusBar->showMessage(err);
}

// Displaying the information about the appliaction
void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this, "About", "This application is used to segment images using seed based method described in paper \"Laplacian Coordinates for Seeded Image Segmentation\" by Casaca, W. ; ICMC, Univ. of Sao Paulo, São Carlos, Brazil ; Nonato, L.G. ; Taubin, G.\n\nImplemented by Kushibar K., Peng S., Veilu Muthu V.");
}

// Displaying the help about the software setting
void MainWindow::on_actionHelp_triggered()
{
    QMessageBox::information(this, "Help", "For more details please visit <a href='https://github.com/vibot-project/s-image-s'>https://github.com/vibot-project/s-image-s</a>.");
}


