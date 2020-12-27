#ifndef HIDDENNETWORK_H
#define HIDDENNETWORK_H

#include <QObject>
#include "network.h"

class HiddenNetwork : public Network
{
        Q_OBJECT
private:
    QString fileSaveData = "/h_etwork_data_";

    int           indexThread = -1;  // индекс НС для удобной работы в потоке
    int           sizeAllTitles = 0; // размер максимального входного вектора
    QList<int>    indexesTitles;     // индексы которы нужно взять из вектора
    QList<double> lastOutput;        // результат последнего предсказания  НС

    bool     trainFinished = true;   // true - если тренеровка завершена. false - если еще идет
    bool     predictFinished = true; // true - если предсказание завершено. false - если еще идет

public:
    // сеттеры
    void setSizeAllTitles(int size);
    void setIndexesTitles(QList<int> indexes);
    void setLastOutput(QList<double> value);
    void setIndexThread(int index);

    // геттеры
    int             getSizeAllTitles();     // получение кол-во всех заголовков (для определения самого первого входного вектора)
    int             getIndexThread();       // получение индекса данной сети в потоке
    QList<int>      getIndexesTitles();     // получение индексов заголовков, которые должна считывать данная НС
    QList<double>   getLastOutput();        // получение послежнего ответа НС (нужно для дальнейшей работы главной НС)

    bool            isTrainFinished();      // сигнал о завершении тренеровки (для интерфейса)
    bool            isPredictFinished();    // сигнал о завершшении предсказаний (для интерфейса)

    explicit HiddenNetwork(QObject *parent = nullptr);
    void init(int indexThread, int sizeInputs, int sizeHidenLayerNods, int sizeHidenLayers, int sizeOutputs, int sizeAllTitles, QList<int> indexesTitles);
    QList<double> train(QList<double> inputs, QList<double> expectedPredict);
    QList<double> predict(QList<double> inputs);
    void save(QString filePath);

public slots:
    void training(int indexThread, QString filePath, qint64 posInFile, qint64 quantityRows, bool *chekFinish);
    void predicting(int indexThread, QList<double> inputs, bool* chekFinish);
    void saving(QString dirPath, int indexThread);
    void open(int indexThread, QString dirPath);
};

#endif // HIDDENNETWORK_H
