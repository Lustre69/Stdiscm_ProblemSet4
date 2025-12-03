#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QProgressBar>
#include <QGridLayout>
#include <QWidget>
#include <QScrollArea>
#include <QLabel>
#include <QFileDialog>
#include <QStringList>
#include <QTimer>
#include <QThread>
#include <QMap>
#include <memory>

#include "ocr_client.h"


// Card widget to display OCR result
class ResultCard : public QWidget {
    Q_OBJECT

public:
    explicit ResultCard(const QString& imageId, QWidget* parent = nullptr);
    void setResult(const QString& text);
    void setInProgress();

private:
    QLabel* mainTextLabel_;
    QLabel* detailLabel_;
    QString imageId_;
};

// Worker thread for handling gRPC communication
class OCRWorkerThread : public QThread {
    Q_OBJECT

public:
    OCRWorkerThread(QObject* parent = nullptr);
    void processImage(const QString& imagePath, const QString& imageId);
    void setClient(std::shared_ptr<OCRClient> client);
    void stop();

signals:
    void resultReady(const QString& imageId, const QString& text, bool success, const QString& error);

private:
    void run() override;
    std::shared_ptr<OCRClient> client_;
    
    struct Task {
        QString imagePath;
        QString imageId;
    };
    
    QQueue<Task> taskQueue_;
    QMutex queueMutex_;
    QWaitCondition condition_;
    bool shouldStop_;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onUploadButtonClicked();
    void onResultReady(const QString& imageId, const QString& text, bool success, const QString& error);
    void onBatchComplete();
    void startNewBatch();

private:
    void setupUI();
    void updateProgressBar();
    void addImageCard(const QString& imageId, const QString& filePath);
    ResultCard* getOrCreateCard(const QString& imageId);
    void clearResults();

    QPushButton* uploadButton_;
    QProgressBar* progressBar_;
    QWidget* scrollWidget_;
    QGridLayout* gridLayout_;
    QScrollArea* scrollArea_;
    
    // Image tracking
    QMap<QString, QString> imagePaths_;  // imageId -> filePath
    QMap<QString, ResultCard*> cards_;   // imageId -> card widget
    QSet<QString> pendingImages_;        // Images waiting for results
    QSet<QString> completedImages_;      // Images with results
    
    int totalImages_;
    int currentBatchStart_;
    
    // gRPC client and worker threads
    std::shared_ptr<OCRClient> ocrClient_;
    QList<OCRWorkerThread*> workerThreads_;
    
    // Server connection settings
    QString serverAddress_;
};

#endif // MAINWINDOW_H

