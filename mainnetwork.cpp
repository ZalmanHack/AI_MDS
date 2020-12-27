#include "mainnetwork.h"

void MainNetwork::waitFinishHiddenPredict()
{
    // ожидание завершения обучения всех НС первого слоя
    //qDebug() << ".";
    forever {
        //QThread::msleep(10);
        bool isFinished = true;
        for(int i = 0; i < listNetworks->size(); i++) {
            if(!listNetworks->at(i)->isPredictFinished()) {
                isFinished = false;
            }
        }
        if(isFinished) {
            qDebug() << "finished";
            break;
        }
    }
}

MainNetwork::MainNetwork(QObject *parent) : Network(parent)
{

}

void MainNetwork::setListHiddenNetwork(QList<HiddenNetwork *> *list)
{
    this->listNetworks = list;
}

void MainNetwork::training(QString filePath)
{
    qRegisterMetaType<QList<double>>("QList<double>");
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
            file.seek(0); // перемещаемся на нужный участок файла, туда где хранятся нужные для этой НС данные
            int row = 0;  // для подсчета кол-ва строк
            while(!file.atEnd())   // читаем файлы и подаем на обучение
            {
                row++;
                // считываем строку из бинарного файла
                stream >> inputs;
                stream >> expectedPredict;

                // создаем массив bool значений и передаем их ссылки НС,
                // которые изменят значения ссылок на true при завершении своих методов
                bool *cheksFinished = new bool[this->getSizeInputs()] {false};
                for(int i = 0; i < getSizeInputs(); i++)
                    emit startHiddenPredict(i, inputs, &(cheksFinished[i]));

                // ожидаем пока все НС ответят
                // будет крутится пока все НС не добавят по 1 к этому числу и оно не будет = их кол-ву
                while(true) {
                    bool isFinished = true;
                    for(int i = 0; i < this->getSizeInputs(); i++) {
                        if(cheksFinished[i] != true){
                            isFinished = false;
                            break;
                        }
                    }
                    if(isFinished == true)
                        break;
                }
                delete[] cheksFinished;
                // ////////////////////////////////////////////////////////////////////////////////////

                qDebug() << "НС 0 Ответила";
                qDebug() << "НС 1 Ответила";
                qDebug() << "НС 2 Ответила";
                qDebug() << "НС 3 Ответила";
                qDebug() << "Строка " <<row << " обработана";

                // /////
                // получаем их ответы
                inputs.clear();
                for(int j = 0; j < listNetworks->size(); j++)
                    inputs.append(listNetworks->at(j)->getLastOutput().at(0));

                // запускаем тренеровку
                QList<double> result = train(inputs, QList<double>() << expectedPredict << fabs(expectedPredict - 1));
                // считаем ошибку сети (возводим в квадрат чтобы посчитать среднюю квадратичную ошибку выходов)
                summ += fabs(expectedPredict - result[0]);
                summ += fabs(fabs(expectedPredict - 1) - result[1]);
            }
            qDebug() << "НС:" << "ГЛАВНАЯ" << "Эпоха: " << epoch << "Ошибка сети: " << (summ * 100) / (row * getSizeOutputs()) << " %." ;

            emit processTrainInfo((summ * 100) / (row * getSizeOutputs()), epoch);
            if((summ * 100) / (row * getSizeOutputs()) <= 100 - getAccuracy()){
                qDebug() << "break";
                break;
            }
        }
        file.close();
    }
}

QList<QList<double>> MainNetwork::prediction(QString filePath)
{
    QList<QList<double>> result;
    qRegisterMetaType<QList<double>>("QList<double>");
    // читаем исходные файлы и записываем их в файл без нормализации
    QFile file(filePath);
    QDataStream stream(&file);
    stream.setVersion(QDataStream::Qt_5_10);
    if(file.open(QIODevice::ReadOnly))
    {
        QList<double> inputs;
        while(!file.atEnd())   // читаем файлы и подаем на обучение
        {
            // считываем строку из бинарного файла
            stream >> inputs;

            // создаем массив bool значений и передаем их ссылки НС,
            // которые изменят значения ссылок на true при завершении своих методов
            bool *cheksFinished = new bool[this->getSizeInputs()] {false};
            for(int i = 0; i < getSizeInputs(); i++)
                emit startHiddenPredict(i, inputs, &(cheksFinished[i]));

            // ожидаем пока все НС ответят
            // будет крутится пока все НС не добавят по 1 к этому числу и оно не будет = их кол-ву
            while(true) {
                bool isFinished = true;
                for(int i = 0; i < this->getSizeInputs(); i++) {
                    if(cheksFinished[i] != true){
                        isFinished = false;
                        break;
                    }
                }
                if(isFinished == true)
                    break;
            }
            delete[] cheksFinished;
            // ////////////////////////////////////////////////////////////////////////////////////

            // получаем их ответы
            inputs.clear();
            for(int j = 0; j < listNetworks->size(); j++)
                inputs.append(listNetworks->at(j)->getLastOutput().at(0));

            // запускаем прогнозирование и получаем 2 ответа (вероятность ДА и НЕТ)
            result.append(predict(inputs));
        }
        file.close();
    }
    return result;
}

void MainNetwork::save(QString dirPath)
{
    Network::save(dirPath + fileSaveData);
}

void MainNetwork::open(QString dirPath)
{
    QFile file(dirPath + fileSaveData);     // создаем файл, куда будем все записывать
    QDataStream stream(&file);                  // класс для потоковой работы с файлом (чтение/запись)
    stream.setVersion(QDataStream::Qt_5_10);    // задаем версию QT для корректного чтения

    if(file.open(QIODevice::ReadOnly)) // только для записи
    {
        QList<QList<QList<double>>> newNetData;
        QList<QList<double>> inputs, outputs;
        // 1) запись размерности НС
        int value;
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
        file.close();                                                                   // Закрываем файл
    }
}
