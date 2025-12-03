#include "mainwindow.h"
#include <QApplication>
#include <QFileInfo>
#include <QUuid>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QTimer>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QSet>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <algorithm>
#include <QMutexLocker>
#include <iostream>

ResultCard::ResultCard(const QString& imageId, QWidget* parent)
    : QWidget(parent), imageId_(imageId)
{
    setFixedSize(200, 150);
    setStyleSheet(
        "QWidget {"
        "background-color: white;"
        "border: 1px solid #ddd;"
        "border-radius: 5px;"
        "}"
    );

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(5);

    mainTextLabel_ = new QLabel(this);
    mainTextLabel_->setStyleSheet(
        "QLabel {"
        "font-size: 16px;"
        "font-weight: bold;"
        "color: black;"
        "background-color: transparent;"
        "}"
    );
    mainTextLabel_->setWordWrap(true);
    mainTextLabel_->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    detailLabel_ = new QLabel(this);
    detailLabel_->setStyleSheet(
        "QLabel {"
        "font-size: 12px;"
        "color: #666;"
        "background-color: transparent;"
        "}"
    );
    detailLabel_->setWordWrap(true);
    detailLabel_->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    layout->addWidget(mainTextLabel_);
    layout->addWidget(detailLabel_);
    layout->addStretch();

    setInProgress();
}

void ResultCard::setResult(const QString& text) {
    QStringList lines = text.split('\n', Qt::SkipEmptyParts);
    
    if (lines.isEmpty()) {
        mainTextLabel_->setText("No text found");
        detailLabel_->setText("");
        return;
    }

    // First line as main text
    QString mainText = lines.first().trimmed();
    if (mainText.length() > 30) {
        mainText = mainText.left(27) + "...";
    }
    mainTextLabel_->setText(mainText);

    // Remaining lines or variations as detail
    if (lines.size() > 1) {
        QString detail = lines.at(1).trimmed();
        detailLabel_->setText(detail);
    } else {
        detailLabel_->setText(text.trimmed());
    }
}

void ResultCard::setInProgress() {
    mainTextLabel_->setText("In progress");
    detailLabel_->setText("");
}

// OCR Worker Thread Implementation
OCRWorkerThread::OCRWorkerThread(QObject* parent)
    : QThread(parent), shouldStop_(false)
{
}

void OCRWorkerThread::setClient(std::shared_ptr<OCRClient> client) {
    client_ = client;
}

void OCRWorkerThread::stop() {
    QMutexLocker locker(&queueMutex_);
    shouldStop_ = true;
    condition_.wakeAll();
}

void OCRWorkerThread::processImage(const QString& imagePath, const QString& imageId) {
    QMutexLocker locker(&queueMutex_);
    Task task;
    task.imagePath = imagePath;
    task.imageId = imageId;
    taskQueue_.enqueue(task);
    condition_.wakeOne();
}

void OCRWorkerThread::run() {
    while (true) {
        Task task;
        
        // Wait for a task
        {
            QMutexLocker locker(&queueMutex_);
            while (taskQueue_.empty() && !shouldStop_) {
                condition_.wait(&queueMutex_);
            }
            
            if (shouldStop_ && taskQueue_.empty()) {
                break;
            }
            
            if (!taskQueue_.empty()) {
                task = taskQueue_.dequeue();
            }
        }

        if (!client_ || task.imagePath.isEmpty()) {
            continue;
        }

        QString text;
        bool success = false;
        QString error;
        
        // Read image file
        QFile file(task.imagePath);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray imageData = file.readAll();
            QString format = QFileInfo(task.imagePath).suffix().toLower();
            
            // Process image through gRPC
            std::string extractedText;
            success = client_->processImage(task.imageId.toStdString(), 
                                            std::string(imageData.constData(), imageData.size()), 
                                            format.toStdString(),
                                            extractedText);
            text = QString::fromStdString(extractedText);
            
            if (!success) {
                error = text.isEmpty() ? "Processing failed" : text;
            }
        } else {
            error = "Could not read image file";
        }

        emit resultReady(task.imageId, text, success, error);
    }
}

// MainWindow Implementation
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , totalImages_(0)
    , currentBatchStart_(0)
    , serverAddress_("localhost:50051")
{
    setupUI();
    
    // Initialize gRPC client
    ocrClient_ = std::make_shared<OCRClient>(serverAddress_.toStdString());
    
    // Create worker threads
    for (int i = 0; i < 4; ++i) {
        OCRWorkerThread* thread = new OCRWorkerThread(this);
        thread->setClient(ocrClient_);
        connect(thread, &OCRWorkerThread::resultReady, this, &MainWindow::onResultReady, Qt::QueuedConnection);
        workerThreads_.append(thread);
        thread->start();
    }
}

MainWindow::~MainWindow()
{
    // Cleanup worker threads
    for (auto* thread : workerThreads_) {
        thread->stop();
        thread->wait();
        delete thread;
    }
}

void MainWindow::setupUI() {
    setWindowTitle("Upload Images");
    setMinimumSize(900, 700);
    resize(1000, 800);

    // Set dark gray background
    setStyleSheet("QMainWindow { background-color: #2b2b2b; }");

    // Central widget
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // Progress bar area
    QHBoxLayout* progressLayout = new QHBoxLayout();
    progressBar_ = new QProgressBar(this);
    progressBar_->setRange(0, 100);
    progressBar_->setValue(0);
    progressBar_->setStyleSheet(
        "QProgressBar {"
        "border: 1px solid #555;"
        "border-radius: 3px;"
        "text-align: center;"
        "background-color: #3b3b3b;"
        "color: white;"
        "}"
        "QProgressBar::chunk {"
        "background-color: #0078d4;"
        "}"
    );
    progressBar_->setMinimumHeight(25);

    uploadButton_ = new QPushButton("Upload Images", this);
    uploadButton_->setStyleSheet(
        "QPushButton {"
        "background-color: #0078d4;"
        "color: white;"
        "border: none;"
        "border-radius: 3px;"
        "padding: 8px 20px;"
        "font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "background-color: #106ebe;"
        "}"
        "QPushButton:pressed {"
        "background-color: #005a9e;"
        "}"
    );

    progressLayout->addWidget(progressBar_, 1);
    progressLayout->addWidget(uploadButton_, 0);
    mainLayout->addLayout(progressLayout);

    // Scroll area for result cards
    scrollArea_ = new QScrollArea(this);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setStyleSheet("QScrollArea { border: none; background-color: #2b2b2b; }");

    scrollWidget_ = new QWidget();
    gridLayout_ = new QGridLayout(scrollWidget_);
    gridLayout_->setSpacing(15);
    gridLayout_->setContentsMargins(10, 10, 10, 10);

    scrollArea_->setWidget(scrollWidget_);
    mainLayout->addWidget(scrollArea_, 1);

    // Connect signals
    connect(uploadButton_, &QPushButton::clicked, this, &MainWindow::onUploadButtonClicked);
}

void MainWindow::onUploadButtonClicked() {
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        "Select Images",
        QDir::homePath(),
        "Image Files (*.png *.jpg *.jpeg *.bmp *.tiff)"
    );

    if (files.isEmpty()) {
        return;
    }

    // Check if we should start a new batch
    if (progressBar_->value() >= 100 && !pendingImages_.empty()) {
        startNewBatch();
    }

    // Add images
    for (const QString& filePath : files) {
        QString imageId = QUuid::createUuid().toString(QUuid::WithoutBraces);
        imagePaths_[imageId] = filePath;
        pendingImages_.insert(imageId);
        totalImages_++;

        // Create card
        addImageCard(imageId, filePath);

        // Add to any worker thread's queue (round-robin or first available)
        // Workers will process from their queues
        static int threadIndex = 0;
        workerThreads_[threadIndex % workerThreads_.size()]->processImage(filePath, imageId);
        threadIndex++;
    }

    updateProgressBar();
}

void MainWindow::addImageCard(const QString& imageId, const QString& filePath) {
    ResultCard* card = new ResultCard(imageId, scrollWidget_);
    cards_[imageId] = card;

    // Add to grid (4 columns)
    int count = cards_.size() - 1;
    int row = count / 4;
    int col = count % 4;
    gridLayout_->addWidget(card, row, col);
}

ResultCard* MainWindow::getOrCreateCard(const QString& imageId) {
    if (cards_.contains(imageId)) {
        return cards_[imageId];
    }
    return nullptr;
}

void MainWindow::onResultReady(const QString& imageId, const QString& text, bool success, const QString& error) {
    pendingImages_.remove(imageId);
    completedImages_.insert(imageId);

    ResultCard* card = getOrCreateCard(imageId);
    if (card) {
        if (success) {
            card->setResult(text);
        } else {
            card->setResult("Error: " + error);
        }
    }

    updateProgressBar();
}

void MainWindow::updateProgressBar() {
    if (totalImages_ == 0) {
        progressBar_->setValue(0);
        return;
    }

    int completed = completedImages_.size();
    int value = (completed * 100) / totalImages_;
    progressBar_->setValue(value);

    // Check if batch is complete
    if (value >= 100 && pendingImages_.empty()) {
        QTimer::singleShot(100, this, &MainWindow::onBatchComplete);
    }
}

void MainWindow::onBatchComplete() {
    // Batch complete - next upload will start new batch
    // Results will be cleared on next upload
}

void MainWindow::startNewBatch() {
    clearResults();
    currentBatchStart_ = totalImages_;
}

void MainWindow::clearResults() {
    // Clear all cards
    for (auto* card : cards_.values()) {
        gridLayout_->removeWidget(card);
        delete card;
    }
    cards_.clear();
    imagePaths_.clear();
    completedImages_.clear();
    pendingImages_.clear();
    totalImages_ = 0;
    progressBar_->setValue(0);
}

