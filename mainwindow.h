#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>/*
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>*/

#include <CAENDigitizer.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QFont *fontUsed;
    QPushButton *startDatasetButton;
    QPushButton *stopDatasetButton;
    bool isStartButtonPressed;

    int handle;
    int ret;
    uint16_t ADC_EVENT_LENGTH;
    uint16_t MAX_NUM_EVENTS;

    uint32_t allocatedSize;
    char *buffer;
    uint32_t numberOfEvents;
    CAEN_DGTZ_EventInfo_t eventInfo;
    char *eventPtr;
    // CAEN_DGTZ_UINT16_EVENT_t *event16;
    CAEN_DGTZ_X742_EVENT_t *event742;

    QtCharts::QLineSeries *series;
    QtCharts::QChart *chart;
    QtCharts::QChartView *chartView;
    QtCharts::QValueAxis *currentAxisX;
    QtCharts::QValueAxis *currentAxisY;

    void showProcess(const char* processName);
    void configureDigitizer();
    void acquireData();

protected:
     void closeEvent(QCloseEvent *event);

private slots:
    void startDatasetSLOT();
    void stopDatasetSLOT();

};
#endif // MAINWINDOW_H
