#include "network.h"

Network::Network(QObject *parent) : QObject(parent)
{

}

void Network::setSizeInputs(int value)
{
    this->sizeInputs = value;
}

void Network::setSizeHidenLayerNods(int value)
{
    this->sizeHidenLayerNods = value;
}

void Network::setSizeHidenLayers(int value)
{
    this->sizeHidenLayers = value;
}

void Network::setSizeOutputs(int value)
{
    this->sizeOutputs = value;
}

void Network::setLearningRate(double value)
{
    this->learning_rate = value;
}

int Network::getQuantityEpoch()
{
    return this->quantityEpoch;
}

QList<QList<QList<double> > > Network::getNetData()
{
    return netData;
}

void Network::setQuantityEpoch(int value)
{
    this->quantityEpoch = value;
}

void Network::setAccuracy(int value)
{
    this->accuracy = value;
}

void Network::setNetData(QList<QList<QList<double> > > data)
{
    this->netData = data;
}

void Network::setInOutTemp(QList<QList<double>> inputs, QList<QList<double>> outputs)
{
    assert(inputs.size() == outputs.size());
    inOutTemp.clear();
    for(int i = 0; i < inputs.size(); i++) {
        inputsOutpts temp;
        temp.inputs = inputs.at(i);
        temp.outputs = outputs.at(i);
        inOutTemp.append(temp);
    }
}

QList<QList<double>> Network::getInOutTempInputs()
{
    QList<QList<double>> result;
    for(inputsOutpts item : inOutTemp)
        result.append(item.inputs);
    return result;
}

QList<QList<double>> Network::getInOutTempOutputs()
{
    QList<QList<double>> result;
    for(inputsOutpts item : inOutTemp)
        result.append(item.outputs);
    return result;
}

int Network::getSizeInputs()
{
   return this->sizeInputs;
}

int Network::getSizeHidenLayerNods()
{
    return this->sizeHidenLayerNods;
}

int Network::getSizeHidenLayers()
{
    return this->sizeHidenLayers;
}

int Network::getSizeOutputs()
{
    return this->sizeOutputs;
}

double Network::getLearningRate()
{
    return this->learning_rate;
}

int Network::getAccuracy()
{
    return this->accuracy;
}

QList<double> Network::dot(QList<QList<double> > matrix, QList<double> vector)
{
    assert(matrix[0].size() == vector.size()); // проверка на верность входных данных чтобы кол-во строк 1й матрицы было равно кл-ву столбцов второй матрицы.
    QList<double> result;
    for(int i = 0; i < matrix.size(); i++) // проходимся по строкам первой матрицы
    {
        result.append(0);
        for(int k = 0; k < matrix.at(i).size(); k++) // поэлементно умножаем и складываем строку со столбцом
            result[i] += matrix[i][k] * vector[k];
    }
    return result;
}

double Network::sigmoid(double x)
{
    return 1.0 / (1 + exp(-x));
}

QList<double> Network::sigmoid_mapper(QList<double> x, int size)
{
    QList<double> result;
    for(int i = 0; i < size; i++)
        result.append(sigmoid(x[i]));
    return result;
}

QList<double> Network::trainAll(QList<double> outputs, int indexOut, QList<double> expectedPredict, int indexExpPred)
{
    assert(outputs.size() == expectedPredict.size());
    // анализ выходного (поиск ошибки и дельты)
    double errorLayer = outputs[indexOut] - expectedPredict[indexExpPred];  // считаем ошибку ответа
    double gradientLayer = outputs[indexOut] * (1.0 - outputs[indexOut]);     // получаем градиент сигмоиды
    double weightsDeltaLayer = errorLayer * gradientLayer;                  // делтьа на которую мы изменим веса
    // обучение внутреннего слоя (рекурсия)
    trainHideLayer(getSizeHidenLayers(), indexOut, 0, weightsDeltaLayer);
    // если есть еще выходы, то переходим к следующему выходу, вызывая эту же функцию (рекурсия)
    if(indexOut < outputs.size() - 1)
        trainAll(outputs, indexOut + 1, expectedPredict, indexExpPred + 1);
    return outputs;
}

void Network::trainHideLayer(int indexHideLayer, int indexNod, int indexWeight, double weightsDeltaLayer)
{
    //(в корне проекта есть КАРТИНКА схема работыобучения)
    assert(indexHideLayer >= 0);
    // если находимся на 1м слове (веса) (помнить, что кол-во слоев с инфой о весах на 1 меньше чем массив выходов и выходов,
    // так как мы не учитываем в netData слой входов, так как у него нет весов)
    if(indexHideLayer == 0) {
        for(int weight = 0; weight < getSizeInputs(); weight++)
            netData[indexHideLayer][indexNod][weight] -= inOutTemp[indexHideLayer].inputs[weight] * weightsDeltaLayer * getLearningRate();
        return;
    }
    //netData на 1 меньше чем inOutTemp
    netData[indexHideLayer][indexNod][indexWeight] -= (inOutTemp[indexHideLayer].outputs[indexWeight] * weightsDeltaLayer) * getLearningRate();
    // считваем градиент, ошибку и дельту для внутреннего слоя (она расчитывается чуть иначе, чем для выходного)
    double errorLayer = weightsDeltaLayer * netData[indexHideLayer][indexNod][indexWeight]; // считаем ошибку ответа для A[i]
    double gradientLayer = inOutTemp[indexHideLayer].outputs[indexWeight] * (1 - inOutTemp[indexHideLayer].outputs[indexWeight]); // получаем градиент сигмоиды
    double weightsDeltaLayerNew = errorLayer * gradientLayer; // делтьа на которую мы умножим веса
    // если слой не первый, то идем дальше к началу (рекурсия)
    if(indexHideLayer > 0)
        trainHideLayer(indexHideLayer - 1, indexWeight, 0, weightsDeltaLayerNew);
    // перебор узлов данного слоя (ркурсия)
    if(indexWeight < getSizeHidenLayerNods() - 1)
        trainHideLayer(indexHideLayer, indexNod, indexWeight + 1, weightsDeltaLayer);
}

QList<double> Network::train(QList<double> inputs, QList<double> expectedPredict)
{
    assert(inputs.size() == getSizeInputs());
    return trainAll(Network::predict(inputs), 0, expectedPredict, 0);
}

QList<double> Network::predict(QList<double> inputs)
{
    assert(inputs.size() == getSizeInputs());
    QList<double> outputsTemp;
    QList<double> inputsTemp;

    inOutTemp[0].inputs = inputs;
    inOutTemp[0].outputs = inputs;
    // вывод информации
    //for(int i = 0; i < sizeInputs; i++)
    //    qDebug() << "A [" << i+1 << "] =" << inOutTemp[0].inputs[i];

    for(int layer = 0; layer < (sizeHidenLayers + OUTPUT); layer++) // проходим по всем слоям + выходной
    {
        //qDebug() << "--- Layer" << layer+1 << "---";
        if(layer == sizeHidenLayers) {  // если слой последний (выходной)
            inputsTemp = dot(netData[layer], outputsTemp); // умножаем все веса слоя на ответы прошлого слоя (умножение матриц)
            outputsTemp = sigmoid_mapper(inputsTemp, sizeOutputs);
            inOutTemp[INPUT + layer].inputs = inputsTemp;
            inOutTemp[INPUT + layer].outputs = outputsTemp;
            // вывод информации
            //for(int i = 0; i < sizeOutputs; i++)
            //    qDebug() << "R [" << i+1 << "] =" << inputsTemp[i] << "   sigmoid =" << outputsTemp[i];
            break;
        }
        if(layer > 0) {
            inputsTemp = dot(netData[layer], outputsTemp);
            outputsTemp = sigmoid_mapper(inputsTemp, sizeHidenLayerNods);
            inOutTemp[INPUT + layer].inputs = inputsTemp;
            inOutTemp[INPUT + layer].outputs = outputsTemp;
            // вывод информации
            //for(int i = 0; i < sizeHidenLayerNods; i++)
            //    qDebug() << "S [" << i+1 << "] =" << inputsTemp[i] << "   sigmoid =" << outputsTemp[i];
            continue;
        }
        if(layer == 0) {    // для первого слоя, чтобы задатькол-во весов == кол-ву узлов INPUT
            inputsTemp = dot(netData[layer], inOutTemp[0].inputs);
            outputsTemp = sigmoid_mapper(inputsTemp, sizeHidenLayerNods);
            inOutTemp[INPUT + layer].inputs = inputsTemp;
            inOutTemp[INPUT + layer].outputs = outputsTemp;
            // вывод информации
            //for(int i = 0; i < sizeHidenLayerNods; i++)
            //    qDebug() << "S [" << i+1 << "] =" << inputsTemp[i] << "   sigmoid =" << outputsTemp[i];
            continue;
        }
    }
    return outputsTemp;
}

void Network::init(int sizeInputs, int sizeHidenLayerNods, int sizeHidenLayers, int sizeOutputs)
{
    /* ИНСТРУКЦИЯ
     * Создаем массив всех скрытых слоев + слой выхода (от слоя веса идут грубо говоря влево)
     * i - номер слоя (hidenLayer)
     * j - номер узла (nod)
     * k - номер веса на j узел от i-1 слоя
     * Пример: кол-во входов = 3; кол-во слоев = 2; кол-ыо узлов в слое = 4; кол-во выходов = 2
     *
     * () - внутренний узел; Столбец из () - внутренний слой
     * [] - выходной узел;   Столбец из [] - выходной слой
     *  # - вес; ряд из # - веса на данный узел
     *
     * ()   (# # #)   (# # # #)
     * ()   (# # #)   (# # # #)   [# # # #]
     * ()   (# # #)   (# # # #)   [# # # #]
     * ()   (# # #)   (# # # #)
     */

    setSizeInputs(sizeInputs);
    setSizeHidenLayerNods(sizeHidenLayerNods);
    setSizeHidenLayers(sizeHidenLayers);
    setSizeOutputs(sizeOutputs);

    srand(QTime::currentTime().msec());                                             // Делаем рандом более рандомным

    inOutTemp.clear();
    netData.clear();

    inputsOutpts temp;
    inOutTemp.append(temp);
    for(int layer = 0; layer < getSizeHidenLayers() + OUTPUT; layer++) {            // Проход по всем слоям кроме первого (входного), его в netData нет, так как у него нет весов
        inOutTemp.append(temp);
        QList<QList<double>> newLayer;
        if(layer == getSizeHidenLayers()) {                                         // Если мы на последнем слое (тот что выходной)
            for(int nod = 0; nod < getSizeOutputs(); nod++) {                       // Перебераем каждый узел выхода
                QList<double> newNod;
                for(int weight = 0; weight < getSizeHidenLayerNods(); weight++)     // Задаем веса данному узлу (рандом от 0.0 до 0.7)
                    newNod.append(static_cast<double>(rand()%71)/100.0);
                newLayer.append(newNod);                                            // добавляем узел в слой
            }
            netData.append(newLayer);
            break;
        }
        if(layer != 0) {                                                            // Если находимся не на 1м слое
            for(int nod = 0; nod < getSizeHidenLayerNods(); nod++) {                // Перебераем каждый узел скрытого слоя
                QList<double> newNod;
                for(int weight = 0; weight < getSizeHidenLayerNods(); weight++)     // Создаем в каждом узле веса (количество весов = кол-ву узлов на внутреннем слое)
                    newNod.append(static_cast<double>(rand()%71)/100.0);
                newLayer.append(newNod);                                            // добавляем узел в слой
            }
            netData.append(newLayer);
            continue;
        }
        else if(layer == 0) {                                                       // Если мы на 1м слое (тот что после входов)
            for(int nod = 0; nod < getSizeHidenLayerNods(); nod++) {                // Перебераем каждый узел скрытого слоя
                QList<double> newNod;
                for(int weight = 0; weight < getSizeInputs(); weight++)             // Создаем в каждом узле веса (количество весов = кол-во входов)
                    newNod.append(static_cast<double>(rand()%71)/100.0);
                newLayer.append(newNod);                                            // добавляем узел в слой
            }
            netData.append(newLayer);
            continue;
        }
    }
}

void Network::save(QString path)
{
    QFile file(path);     // создаем файл, куда будем все записывать
    QDataStream stream(&file);                  // класс для потоковой работы с файлом (чтение/запись)
    stream.setVersion(QDataStream::Qt_5_10);    // задаем версию QT для корректного чтения
    if(file.open(QIODevice::WriteOnly)) // только для записи
    {
        // 1) запись размерности НС
        stream << getSizeInputs();          // запись размера входоы
        stream << getSizeHidenLayers();     // запись кол-ва скрытых слоев
        stream << getSizeHidenLayerNods();  // запись кол-ва узлов для каждого скрытого слоя
        stream << getSizeOutputs();         // запись кол-ва узлов выходного слоя
        // 2) запись весов НС
        stream << getNetData();
        stream << getInOutTempInputs();
        stream << getInOutTempOutputs();
        file.flush();
        file.close();                                                                   // Закрываем файл
    }
}

void Network::setSettingsNetworks(int epoch, double learningRate, int accuracy)
{
    setLearningRate(learningRate);     // скорость обучения
    setQuantityEpoch(epoch);
    setAccuracy(accuracy);
}
