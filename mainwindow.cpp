#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ADC_EVENT_LENGTH = 1024;
    MAX_NUM_EVENTS = 124;
    buffer = NULL;
    numberOfEvents = 0;
    eventPtr = NULL;
    event742 = NULL;
    configureDigitizer();
    ret = CAEN_DGTZ_AllocateEvent(handle, (void**)&event742); showProcess("AllocateEvent");
    ret = CAEN_DGTZ_MallocReadoutBuffer(handle, &buffer, &allocatedSize); showProcess("MallocReadoutBuffer");

    fontUsed = new QFont();
    fontUsed->setBold(true);

    fontUsed->setPointSize(24);
    startDatasetButton = new QPushButton("Start", this);
    startDatasetButton->
            setStyleSheet("QPushButton{background-color:#F0E68C;border: 0px;}QPushButton:pressed{background-color:#E0FFFF;border:0px;}");
    startDatasetButton->setGeometry(QRect(QPoint(10, 90), QSize(140, 46)));
    startDatasetButton->setFont(*fontUsed);
    stopDatasetButton = new QPushButton("Stop", this);
    stopDatasetButton->
            setStyleSheet("QPushButton{background-color:#F0E68C;border: 0px;}QPushButton:pressed{background-color:#E0FFFF;border:0px;}");
    stopDatasetButton->setGeometry(QRect(QPoint(10, 140), QSize(140, 46)));
    stopDatasetButton->setFont(*fontUsed);

    series= new QLineSeries();

    currentAxisX = new QValueAxis();
    currentAxisY = new QValueAxis();

    chart = new QChart();
    chart->legend()->hide();
    chart->setTitle("Oscillogram");
    fontUsed->setPointSize(18);
    chart->setTitleFont(*fontUsed);
    chart->QChart::addAxis(currentAxisX, Qt::AlignBottom);
    chart->QChart::addAxis(currentAxisY, Qt::AlignLeft);

    chart->addSeries(series);
    series->attachAxis(currentAxisX);
    series->attachAxis(currentAxisY);
    series->setColor(QColor(0, 0, 0, 255));// black, черный
    currentAxisX->setRange(0, 1024);
//    currentAxisY->setRange(-0.5, 0.5);
    currentAxisY->setRange(0.0, 0.02);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    // chartView->setRubberBand(QChartView::RectangleRubberBand);
    chartView->resize(1280, 720);
    chartView->setParent(this);
    chartView->move(200, 0);
    chartView->viewport()->installEventFilter(this);
    chartView->viewport()->setMouseTracking(false);

    connect(startDatasetButton, SIGNAL(clicked()), this, SLOT(startDatasetSLOT()));
    connect(stopDatasetButton, SIGNAL(clicked()), this, SLOT(stopDatasetSLOT()));
}

void MainWindow::showProcess(const char* processName)
{
    if(ret)
    {
        std::cout << "Error while " << processName << std::endl;
        isStartButtonPressed = false;
    }
    else { std::cout << processName << " completed successfully" << std::endl; }
}

void MainWindow::configureDigitizer()
{
    CAEN_DGTZ_BoardInfo_t BoardInfo;
    ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB, 0, 0, 0, &handle);
    if(ret)
    {
        std::cout << "Can not open digitizer" << std::endl;
        //exit(0);
    }
    ret = CAEN_DGTZ_GetInfo(handle, &BoardInfo); showProcess(BoardInfo.ModelName);
    ret = CAEN_DGTZ_Reset(handle); showProcess("Reset");
    ret = CAEN_DGTZ_SetDRS4SamplingFrequency(handle, CAEN_DGTZ_DRS4_1GHz); showProcess("SetDRS4SamplingFrequency");
    ret = CAEN_DGTZ_SetRecordLength(handle, ADC_EVENT_LENGTH); showProcess("SetRecordLength");
//    ret = CAEN_DGTZ_SetPostTriggerSize(handle, (uint32_t)70); showProcess("SetPostTriggerSize");
//    ret = CAEN_DGTZ_SetExtTriggerInputMode(handle, CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT); showProcess("SetExtTriggerInputMode");
    ret = CAEN_DGTZ_SetMaxNumEventsBLT(handle, MAX_NUM_EVENTS); showProcess("SetMaxNumEventsBLT");
    ret = CAEN_DGTZ_SetAcquisitionMode(handle, CAEN_DGTZ_SW_CONTROLLED); showProcess("SetAcquisitionMode");
    ret = CAEN_DGTZ_LoadDRS4CorrectionData(handle, CAEN_DGTZ_DRS4_1GHz); showProcess("LoadDRS4CorrectionData");
    ret = CAEN_DGTZ_EnableDRS4Correction(handle); showProcess("EnableDRS4Correction");
}

void MainWindow::acquireData()
{
    // uint16_t Amplitude = 0;
    // uint16_t numberOfGroups = 2;
    // uint16_t numberOfChannels = 8;

    // ret = CAEN_DGTZ_ClearData(handle); showProcess(ret, "ClearData");
//    ret = CAEN_DGTZ_AllocateEvent(handle, (void**)&event742); showProcess("AllocateEvent");
//    ret = CAEN_DGTZ_MallocReadoutBuffer(handle, &buffer, &allocatedSize); showProcess("MallocReadoutBuffer");
//    ret = CAEN_DGTZ_SWStartAcquisition(handle); showProcess("SWStartAcquisition");
//    ret = CAEN_DGTZ_SendSWtrigger(handle); showProcess("SendSWtrigger");
    ret = CAEN_DGTZ_ReadData(handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer, &allocatedSize); showProcess("ReadData");
    ret = CAEN_DGTZ_GetNumEvents(handle, buffer, allocatedSize, &numberOfEvents); showProcess("GetNumEvents");
    ret = CAEN_DGTZ_GetEventInfo(handle, buffer, allocatedSize, numberOfEvents - 1, &eventInfo, &eventPtr); showProcess("GetEventInfo");
    ret = CAEN_DGTZ_DecodeEvent(handle, eventPtr, (void**)&event742); showProcess("DecodeEvent");
std::cout << numberOfEvents << std::endl;
//    for (uint16_t i = 0; i < numberOfGroups; i++)
//    {
//        for (uint16_t j = 0; j < numberOfChannels; j++)
//        {
//            for (uint16_t k = 0; k < ADC_EVENT_LENGTH ; k++)
//            {
//                Amplitude = (uint16_t)(event742->DataGroup[i].DataChannel[j][k]);
//            }
//        }
//    }
//    if(!ret)
//    {
//        series->clear();
//        chart->removeSeries(series);
//        for (uint16_t k = 0; k < ADC_EVENT_LENGTH ; k++)
//        {
//            series->append(k, (uint16_t)(event742->DataGroup[0].DataChannel[0][k] ) / (float)4096.0 - (float)0.5);
//        }
//        chart->addSeries(series);
//        series->attachAxis(currentAxisX);
//        series->attachAxis(currentAxisY);
//    }

//    ret = CAEN_DGTZ_SWStopAcquisition(handle); showProcess("SWStopAcquisition");
//    ret = CAEN_DGTZ_FreeEvent(handle, (void**)&event742); showProcess("FreeEvent");
//    ret = CAEN_DGTZ_FreeReadoutBuffer(&buffer); showProcess("FreeReadoutBuffer");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    ret = CAEN_DGTZ_FreeEvent(handle, (void**)&event742); showProcess("FreeEvent");
    ret = CAEN_DGTZ_FreeReadoutBuffer(&buffer); showProcess("FreeReadoutBuffer");
    ret = CAEN_DGTZ_CloseDigitizer(handle);
    if(ret) { std::cout << "Can not close digitizer!" << std::endl; }
    else { std::cout << "Closing digitizer.." << std::endl; }
}

void MainWindow::startDatasetSLOT()
{
    ret = CAEN_DGTZ_SWStartAcquisition(handle); showProcess("SWStartAcquisition");
    for(int i = 0; i < 124; i++)
    {
        ret = CAEN_DGTZ_SendSWtrigger(handle); showProcess("SendSWtrigger");
    }
    isStartButtonPressed = true;// std::cout << isStartButtonPressed << std::endl;
//    while(isStartButtonPressed) { acquireData(); }
    acquireData();
}

void MainWindow::stopDatasetSLOT()
{
    ret = CAEN_DGTZ_SWStopAcquisition(handle); showProcess("SWStopAcquisition");
    isStartButtonPressed = false;// std::cout << isStartButtonPressed << std::endl;
}

MainWindow::~MainWindow()
{
    delete ui;
}
