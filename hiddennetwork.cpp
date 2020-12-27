#include "hiddennetwork.h"

HiddenNetwork::HiddenNetwork(QObject *parent) : Network(parent)
{

}

void HiddenNetwork::setSizeAllTitles(int size)
{
    this->sizeAllTitles = size;
}

void HiddenNetwork::setIndexesTitles(QList<int> indexes)
{
    this->indexesTitles = indexes;
}

void HiddenNetwork::setLastOutput(QList<double> value)
{
    this->lastOutput = value;
}

void HiddenNetwork::setIndexThread(int index)
{
    this->indexThread = index;
}

int HiddenNetwork::getSizeAllTitles()
{
    return this->sizeAllTitles;
}

int HiddenNetwork::getIndexThread()
{
    return this->indexThread;
}

QList<int> HiddenNetwork::getIndexesTitles()
{
    return this->indexesTitles;
}

QList<double> HiddenNetwork::getLastOutput()
{
    return this->lastOutput;
}

bool HiddenNetwork::isTrainFinished()
{
    return this->trainFinished;
}

bool HiddenNetwork::isPredictFinished()
{
    return this->predictFinished;
}

void HiddenNetwork::init(int indexThread, int sizeInputs, int sizeHidenLayerNods, int sizeHidenLayers, int sizeOutputs, int sizeAllTitles, QList<int> indexesTitles)
{
    setIndexThread(indexThread);
    setSizeAllTitles(sizeAllTitles);
    setIndexesTitles(indexesTitles);

    qDebug() << "size inputs in HiddenNetwork::init" << sizeInputs << sizeAllTitles;

    Network::init(sizeInputs, sizeHidenLayerNods, sizeHidenLayers, sizeOutputs);
}

QList<double> HiddenNetwork::train(QList<double> inputs, QList<double> expectedPredict)
{
    assert(inputs.size() == getSizeAllTitles()); // необходимо чтобы входной вектор был всегда равным заданному максимальному                                                // это необходимо для корректной работы отбора нудных индексов
    // отбираем только нужнфе индексы и после этого подаем на тренеровку
    QList<double> result;
    for(int index : indexesTitles)
        result.append(inputs.at(index));
    //std::sort(result.begin(), result.end());        // сортируем
    return Network::train(result, expectedPredict); // возвращаем ответ после обучения
}

QList<double> HiddenNetwork::predict(QList<double> inputs)
{
    assert(inputs.size() == getSizeAllTitles()); // необходимо чтобы входной вектор был всегда равным заданному максимальному
                                                 // это необходимо для корректной работы отбора нудных индексов
    // отбираем только нужнфе индексы и после этого подаем на анализ
    QList<double> result;
    for(int index : indexesTitles)
        result.append(inputs.at(index));
    std::sort(result.begin(), result.end());    // сортируем
    result = Network::predict(result);          // оцениваем
    setLastOutput(result);                      // запоминаем последний ответ
    return result;
}

void HiddenNetwork::saving(QString dirPath, int indexThread)
{
    if(indexThread == this->indexThread)
        save(dirPath + fileSaveData + QString::number(indexThread) + ".bin");
}

void HiddenNetwork::save(QString filePath)
{
    QFile file(filePath);     // создаем файл, куда будем все записывать
    QDataStream stream(&file);                  // класс для потоковой работы с файлом (чтение/запись)
    stream.setVersion(QDataStream::Qt_5_10);    // задаем версию QT для корректного чтения
    if(file.open(QIODevice::WriteOnly)) { // добавление инфы
        // 1) запись размерности НС
        stream << getSizeInputs();          // запись размера входоы
        stream << getSizeHidenLayers();     // запись кол-ва скрытых слоев
        stream << getSizeHidenLayerNods();  // запись кол-ва узлов для каждого скрытого слоя
        stream << getSizeOutputs();         // запись кол-ва узлов выходного слоя
        // 2) запись весов НС
        stream << getNetData();
        stream << getInOutTempInputs();
        stream << getInOutTempOutputs();
        // 3) дополнительные данные для этого типа НС
        stream << getSizeAllTitles();
        stream << getIndexesTitles();
        stream << getIndexThread();
        file.close();
    }
}

void HiddenNetwork::open(int indexThread, QString dirPath)
{
    if(indexThread == this->indexThread) // индекс должен бын задан заранее ОБЯЗАТЕЛЬНО!
    {
        QFile file(dirPath + fileSaveData + QString::number(indexThread) + ".bin");     // создаем файл, куда будем все записывать
        QDataStream stream(&file);                  // класс для потоковой работы с файлом (чтение/запись)
        stream.setVersion(QDataStream::Qt_5_10);    // задаем версию QT для корректного чтения
        if(file.open(QIODevice::ReadOnly)) // только для записи
        {
            // 1) запись размерности НС
            int value;
            QList<QList<QList<double>>> newNetData;
            QList<int> indexes;
            QList<QList<double>> inputs, outputs;
            stream >> value;
            setSizeInputs(value);          // запись размера входоы
            stream >> value;
            setSizeHidenLayers(value);     // запись кол-ва скрытых слоев
            stream >> value;
            setSizeHidenLayerNods(value);  // запись кол-ва узлов для каждого скрытого слоя
            stream >> value;
            setSizeOutputs(value);         // запись кол-ва узлов выходного слоя
            // 2) запись весов НС
            stream >> newNetData;
            setNetData(newNetData);
            stream >> inputs;
            stream >> outputs;
            setInOutTemp(inputs, outputs);
            // 3) дополнительные данные для этого типа НС
            stream >> value;
            setSizeAllTitles(value);
            stream >> indexes;
            setIndexesTitles(indexes);
            stream >> value;
            setIndexThread(value);
            file.close();                                                                   // Закрываем файл
        }
    }
}

void HiddenNetwork::training(int indexThread, QString filePath, qint64 posInFile, qint64 quantityRows, bool *chekFinish)
{
    if(indexThread == this->indexThread) // если индекс сети совподает со входящим сигналом
    {
        // читаем исходные файлы и записываем их в файл без нормализации
        QFile file(filePath);
        QDataStream stream(&file);
        stream.setVersion(QDataStream::Qt_5_10);
        if(file.open(QIODevice::ReadOnly))
        {
            QList<double> inputs;
            double expectedPredict;
            double summ = 0; // сумма ответов сети, для расчета квадратичной ошибки
            for(int epoch = 0; epoch < getQuantityEpoch(); epoch++) // крутимся до тех пор пока погрешность не мтпн < 1 процента
            {
                summ = 0;
                file.seek(posInFile); // перемещаемся на нужный участок файла, туда где хранятся нужные для этой НС данные
                for(int i = 0; i < quantityRows; i++)   // читаем файлы и подаем на обучение
                {
                    // считываем строку из бинарного файла
                    stream >> inputs;
                    stream >> expectedPredict;
                    // запускаем тренеровку
                    QList<double> result = train(inputs, QList<double>() << expectedPredict);
                    // считаем ошибку сети (возводим в квадрат чтобы посчитать среднюю квадратичную ошибку выходов)
                    summ += fabs(expectedPredict - result[0]);
                }
                qDebug() << "НС:" << indexThread << "Эпоха: " << epoch << "Ошибка сети: " << (summ * 100) / (quantityRows * getSizeOutputs()) << " %." ;

                //emit processTrainInfo(indexThread, (summ * 100) / (quantityRows * getSizeOutputs()));
                if((summ * 100) / (quantityRows * getSizeOutputs()) <= 100 - getAccuracy()){ // accuracy - точность обучения. вычитаем из 100 чтобы получить ошибку
                    qDebug() << "break";
                    break;
                }
            }
            file.close();
        }
        *chekFinish = !*chekFinish;
    }
}

void HiddenNetwork::predicting(int indexThread, QList<double> inputs, bool* chekFinish)
{
    if(indexThread == this->indexThread)
    {
        predict(inputs);
        *chekFinish = !*chekFinish; // менняя этот парметр мы выходим из цикла ожидания там где вызывали
    }
}
