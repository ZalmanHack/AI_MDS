#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <assert.h>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QTime>
#include <QDebug>

class Network : public QObject
{
    Q_OBJECT

private:
    enum layer {INPUT = 1, OUTPUT = 1};
    QString saveFile = "/network_data.bin";
    struct inputsOutpts {
        QList<double> inputs;
        QList<double> outputs;
    };
    QList<inputsOutpts> inOutTemp;


    int sizeInputs = 0;                 // Количество входов НС
    int sizeHidenLayerNods = 0;         // Количество узлов внутренних слоев
    int sizeHidenLayers = 0;            // Количество внутренних слоев
    int sizeOutputs = 0;                // Количество выходных узлов
    double learning_rate = 0.05;        // скорость обучения
    QList<QList<QList<double>>> netData;// веса всей нейронки
    int quantityEpoch = 2000;           // максимальное кол-во эпох
    int accuracy = 95;                  // точность обучения НС (используется в дочерних классах)

    // функция активации
    double sigmoid(double x);
    // функция активации, которая проходит через все узлы слоя
    QList<double> sigmoid_mapper(QList<double> x, int size);
    // Умножение матриц
    QList<double> dot(QList<QList<double> > matrix, QList<double> vector);
    // рекурсивные методы для обучения НС
    QList<double> trainAll(QList<double> outputs, int indexOut, QList<double> expectedPredict, int indexExpPred);
    void trainHideLayer(int indexHideLayer, int indexNod, int indexWeight, double weightsDeltaLayer);

public:

    // сетеры
    void setSizeInputs(int value);          // размер входного вектора
    void setSizeHidenLayerNods(int value);  // кол-во узлов для скрытых слоев
    void setSizeHidenLayers(int value);     // кол-во скрытых слоев
    void setSizeOutputs(int value);         // кол-во узлов на выходном слое
    void setLearningRate(double value);     // скорость обучения
    void setQuantityEpoch(int value);       // количество эпох
    void  setAccuracy(int value);            // желаемая точность обучения
    void setNetData(QList<QList<QList<double>>> data);


    // геттеры
    int getSizeInputs();                    // размер входного вектора
    int getSizeHidenLayerNods();            // кол-во узлов для скрытых слоев
    int getSizeHidenLayers();               // кол-во скрытых слоев
    int getSizeOutputs();                   // кол-во узлов на выходном слое
    double getLearningRate();               // скорость обучения
    int    getAccuracy();                   // желаемая точность обучения
    int    getQuantityEpoch();              // количество эпох
    QList<QList<QList<double>>> getNetData();

    // для сохранения в файл и открытия из файла
    void setInOutTemp(QList<QList<double>> inputs, QList<QList<double>> outputs);
    QList<QList<double>> getInOutTempInputs();     // получение всех inputs из inOutTemp
    QList<QList<double>> getInOutTempOutputs();    // получение всех outputs из inOutTemp


    explicit Network(QObject *parent = nullptr);
    // обучение
    virtual QList<double> train(QList<double> inputs, QList<double> expectedPredict);
    // предсказание
    virtual QList<double> predict(QList<double> inputs);

    virtual void init(int sizeInputs, int sizeHidenLayerNods, int sizeHidenLayers, int sizeOutputs);
    virtual void save(QString dirPath);

signals:

public slots:
    void setSettingsNetworks(int epoch, double learningRate, int accuracy); // задание настроек сети
};

#endif // NETWORK_H
