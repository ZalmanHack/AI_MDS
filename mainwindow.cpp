#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ИНТЕРФЕЙС --------
    {
    // создание виджета отображающего прогресс обучения
    graphTrain = new GraphTrain(ui->widgetFromGraphTrain);
    graphTrain->setInfo(50.5, 71);
    ui->menuPredict->raise();

    ui->widgetTrainStarted->setVisible(false);
    ui->widgetTrainSettings->setVisible(false);
    ui->widgetPredict->setVisible(false);
    }

    // ЛОГИКА -----------
    {
    mainThread = new QThread(this);
    mainProcess = new MainProcess();
    mainProcess->moveToThread(mainThread);

    connect(this, SIGNAL(destroyed(QObject*)), mainThread, SLOT(quit()));  // правильно завершаем процесс пр срабатывании диструктора главного окна (работает не очень хорошо)
    connect(this, SIGNAL(startLoadData(QString)), mainProcess, SLOT(loadData(QString)));    // загрузка (создание датасета)
    connect(this, SIGNAL(startTraining()), mainProcess, SLOT(training()));                  // начало обучения
    connect(this, SIGNAL(startPredict(QString)), mainProcess, SLOT(predict(QString)));      // предсказание
    connect(this, SIGNAL(startSeving()), mainProcess, SLOT(seving()));                      // сохранения данных обучения
    connect(this, SIGNAL(startInit()), mainProcess, SLOT(initDataInFile()));                // полная инициализация из файлов
    connect(this, SIGNAL(setSettingsNetworks(int,double,int)), mainProcess, SIGNAL(setSettingsNetworks(int, double, int))); // передаем настройки НС

    // сигналы о завершении работы процессов
    connect(mainProcess, SIGNAL(loadDataFinished(bool)), this, SLOT(loadDataFinished(bool)));
    connect(mainProcess, SIGNAL(trainingMainFinished(bool)), this, SLOT(trainingMainFinished(bool)));
    connect(mainProcess, SIGNAL(initFinished(bool)), this, SLOT(initFinished(bool)));
    connect(mainProcess, SIGNAL(sendTrainInfo(double,int)), graphTrain, SLOT(setInfo(double,int))); //передаем инфу на график обучения

    mainThread->start();    // запускаем поток главного процесса
    emit startInit();       // подаем сигнал для начала инициализации из файла
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
        ui->widgetMenu->setGeometry(0,0,this->width(), this->height());
        ui->widgetTrainSettings->setGeometry(0,0,this->width(), this->height());
        ui->widgetTrainStarted->setGeometry(0,0,this->width(), this->height());
        ui->widgetPredict->setGeometry(0,0,this->width(), this->height());
}

void MainWindow::loadDataFinished(bool value)
{
    if(value) {
        emit setSettingsNetworks(ui->epochTrain->value(), ui->speedTrain->value(), ui->accuracyTrain->value());
        // запускаем анимацию подготовки данных (будет крутится до тех пор пока первый слой не обучится
        ui->widgetTrainStarted->setVisible(true);
        ui->widgetTrainSettings->setVisible(false);
        graphTrain->startWaiting();
        QThread::msleep(500); // ждем пока настройки передадутся чтобы не мучаться со сигналами через 3 класса
        emit startTraining();   // отправляем сигнал на начало обучения
    }
}


void MainWindow::trainingMainFinished(bool value)
{
    if(value) {
        emit startSeving(); // чигнал на сохранение НС
        QMessageBox::information(nullptr, "Обучение завершено", "Обучение успешно завершено!");
        ui->widgetTrainStarted->setVisible(false);
        graphTrain->stopWaiting();
        ui->widgetTrainSettings->setVisible(false);
        ui->widgetMenu->setVisible(true);
        ui->menuPredict->setVisible(true);
    }
    else {
        QMessageBox::critical(nullptr, "Обучение не завершено", "Обучение не может быть завершено.\n"                                                             "Неизвестная ошибка!");
        ui->widgetTrainStarted->setVisible(false);
        graphTrain->stopWaiting();
        ui->widgetTrainSettings->setVisible(true);
        ui->widgetMenu->setVisible(false);
        ui->menuPredict->setVisible(false);
    }
}

void MainWindow::trainingHidenFinished(bool value)
{
    graphTrain->stopWaiting();
}

void MainWindow::initFinished(bool value)
{
    // если инициализация прошла удачно
    ui->menuPredict->setVisible(value);
    ui->widgetMenu->setVisible(value);
    ui->widgetTrainSettings->setVisible(!value);
}

void MainWindow::on_buttonOpenTrain_clicked() // кнопка для открытия раздела тренеровки
{
    ui->widgetMenu->setVisible(false);
    ui->widgetTrainSettings->setVisible(true);
}

void MainWindow::on_OpenMenu_1_clicked()
{
    ui->widgetMenu->setVisible(true);
    ui->widgetTrainSettings->setVisible(false);
}

void MainWindow::on_buttonStartTrain_clicked()
{
    QString dirPath = QFileDialog::getExistingDirectory();
    if(!dirPath.isEmpty()){
        emit startLoadData(dirPath);
    }
}

void MainWindow::on_buttonOpenPredict_clicked()
{
    ui->widgetMenu->setVisible(false);
    ui->widgetPredict->setVisible(true);

    ui->saveWidget->setVisible(false);
}

void MainWindow::on_buttonOpenMenu_2_clicked()
{
    ui->widgetPredict->setVisible(false);
    ui->widgetMenu->setVisible(true);
}
