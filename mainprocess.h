#ifndef MAINPROCESS_H
#define MAINPROCESS_H

#include <QObject>
#include <QThread>

#include "MainNetwork.h"
#include "hiddennetwork.h"
#include "processcsv.h"

class MainProcess : public QObject
{
    Q_OBJECT
private:
    ProcessCSV *processCSV = nullptr;   // процесс создающий датасет
    QList<HiddenNetwork*> listNetworks; // список НС певого слоя
    QList<QThread*> listThreads;        // список потоков
    MainNetwork *mainNetwork = nullptr; // главная (результирующая) НС

    void deleteNetworks();
    void initNetworks();
    void initNetworks(QString dirPath);
    void trainingHidden(); // обучение скрытого первого слоя


public:
    explicit MainProcess(QObject *parent = nullptr);
    ~MainProcess();

signals:
    // сигналы для НС
    void startTrainingHiddenNetwork(int indexThread, QString filePath, qint64 posInFile, qint64 quantityRows, bool *chekFinih); // начало обучения каждой НС первого слоя
    void startPredictHiddenNetwork(QList<double> inputs);
    void startSave(QString dirPath, int indexThread);
    void sendTrainInfo(double,int); // сигнал с инфой о пройденой эпохе обучения, данные получены с mainNetwork через сигнал

    // сигналы о завершении работы какой-либо задачи
    void loadDataFinished(bool); // bool отвечает за корректность завершения метода
    void trainingHidenFinished(bool);
    void trainingMainFinished(bool);
    void initFinished(bool);        // сигнал о корректности инициализации из файла
    void setSettingsNetworks(int epoch,double learningRate,int accuracy);// передача настроек в НС всех уровней!
public slots:
    void loadData(QString dirPath); // создание датасета  
    void training();                // обучение всей НС
    void predict(QString filePath); // предсказание
    void seving();
    void initDataInFile(); // инициализация НС из ранее созданного файла
};

#endif // MAINPROCESS_H
