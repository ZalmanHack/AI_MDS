#ifndef MAINNETWORK_H
#define MAINNETWORK_H

#include <QObject>
#include <QThread>

#include "network.h"
#include "hiddennetwork.h"

class MainNetwork : public Network
{
    Q_OBJECT
private:
    QString fileSaveData = "/m_network_data.bin";
    void waitFinishHiddenPredict(); // ожидание ответа всех НС первого слоя
    QList<HiddenNetwork*> *listNetworks = nullptr;

public:
    explicit MainNetwork(QObject *parent = nullptr);
    // получаем список сетей чтобы можно было проверять их готовность без сигналов
    void setListHiddenNetwork(QList<HiddenNetwork*> *list);


public slots:
    void training(QString filePath);
    QList<QList<double>> prediction(QString filePath);
    void save(QString dirPath);
    void open(QString dirPath);

signals:
    // сигнал НС первого слоя, чтобы они выдали свой результат на вход главной нС
    void startHiddenPredict(int indexThread, QList<double>, bool *chekFinish);
    void processTrainInfo(double errorTrain, int epoch);
};

#endif // MAINNETWORK_H
