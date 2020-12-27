#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "graphtrain.h"

#include <QMainWindow>
#include <QThread>
#include <QFileDialog>
#include <QMessageBox>
#include <QPropertyAnimation>

#include "mainprocess.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::MainWindow *ui;
    GraphTrain *graphTrain;                 // UI графика тренеровки НС
    QThread *mainThread;
    MainProcess *mainProcess;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_buttonOpenTrain_clicked();
    void on_OpenMenu_1_clicked();
    void on_buttonStartTrain_clicked();
    void on_buttonOpenPredict_clicked();
    void on_buttonOpenMenu_2_clicked();

protected:
    void resizeEvent(QResizeEvent *event) override;

signals:
    void startLoadData(QString dirPath);    // начать загрузку данных для обучения
    void startTraining();                   // начать тренеровки
    void startPredict(QString filePath);    // начать предсказывание
    void startSeving();                     // начать сохранение данных обучения
    void startInit();                       // начать инициализацию из файла
    void setSettingsNetworks(int epoch,double learningRate,int accuracy);// передача настроек сети

public slots:
    void loadDataFinished(bool value); // проверка корректности создания файла для обучения
    void trainingMainFinished(bool value); // проверка завершения обучения (value отвечает за вкл и выкл кнопок в момент обучения)
    void trainingHidenFinished(bool value);
    void initFinished(bool value);     // проверка корректности инициализации виз файла
};

#endif // MAINWINDOW_H
