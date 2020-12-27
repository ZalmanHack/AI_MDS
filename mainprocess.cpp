#include "mainprocess.h"

#include "hiddennetwork.h"

MainProcess::MainProcess(QObject *parent) : QObject(parent)
{
    processCSV = new ProcessCSV(this);
    // запуск проверки и удаления временных файлов
    processCSV->deleteTempFiles();
}

MainProcess::~MainProcess()
{
    processCSV->deleteTempFiles();
}

void MainProcess::deleteNetworks()
{
    // удаляем гланую НС
    if(mainNetwork != nullptr)
        mainNetwork->deleteLater();
    // если потоки уже были созданы то удаляем все
    if(!listThreads.isEmpty()) {
        for(int i = 0; i < listThreads.size(); i++) {
            listThreads.at(i)->terminate(); // аварийоно завершаем процесс (поток)
            listThreads.at(i)->deleteLater();
            listNetworks.at(i)->deleteLater();
        }
        listThreads.clear();
        listNetworks.clear();
    }
}

void MainProcess::initNetworks()
{
    // инициализация главной НС

    mainNetwork = new MainNetwork();
    int sizeHidenLayerNods;
    if(processCSV->getQuantityNetworks() <= 4)
        sizeHidenLayerNods = 2;
    else
        sizeHidenLayerNods = processCSV->getQuantityNetworks() / 2;
    mainNetwork->init(processCSV->getQuantityNetworks(),  sizeHidenLayerNods, 1, 2);

    // инициализация НС первого слоя
    for(int i = 0; i < processCSV->getQuantityNetworks(); i++)
    {
        HiddenNetwork *newNetwork = new HiddenNetwork();
        // считаем кол-во узлов для каждой НС относительно кол-ва ее входов
        if(processCSV->getQuantityInputs().at(i) <= 4)
            sizeHidenLayerNods = 2;
        else
            sizeHidenLayerNods = processCSV->getQuantityInputs().at(i) / 2;
        newNetwork->init(i, processCSV->getQuantityInputs().at(i), sizeHidenLayerNods, 1, 1, processCSV->getQuantityMainInputs(), processCSV->getIndexesTitles().at(i));

        QThread *newThread = new QThread(this);
        newNetwork->moveToThread(newThread);
        newThread->start();

        listNetworks.append(newNetwork);
        listThreads.append(newThread);

        // сигнал на завершение потока
        connect(this, SIGNAL(destroyed(QObject*)), newThread, SLOT(quit()));
        // сигнал на начало обучения НС
        connect(this, SIGNAL(startTrainingHiddenNetwork(int,QString,qint64,qint64,bool*)), newNetwork, SLOT(training(int,QString,qint64,qint64,bool*)));
        // сигнал на начало оценивания данных строки
        connect(mainNetwork, SIGNAL(startHiddenPredict(int, QList<double>,bool*)), newNetwork, SLOT(predicting(int, QList<double>,bool*)));
        // сигнал с данными о пройденной эпохе тренировки
        connect(mainNetwork, SIGNAL(processTrainInfo(double,int)), this, SIGNAL(sendTrainInfo(double,int))); // отправляем инфу об обучении спустя 1 эпоху
        // сохранение НС
        connect(this, SIGNAL(startSave(QString,int)), newNetwork, SLOT(saving(QString,int)));
        // получение настроек сети
        connect(this, SIGNAL(setSettingsNetworks(int,double,int)), newNetwork, SLOT(setSettingsNetworks(int,double,int)));
    }

    // передаем ссылку насписок НС первого слоя, чтобы иметь к ним прямой доступ (поток от этого не страдает)
    mainNetwork->setListHiddenNetwork(&listNetworks);
    connect(this, SIGNAL(setSettingsNetworks(int,double,int)), mainNetwork, SLOT(setSettingsNetworks(int,double,int)));
}

void MainProcess::loadData(QString dirPath)
{
    // вызываем метод создания датасета, он вернет true если сможет выделить категории
    // ловим ответ этого метода и отправляем сигнал в mainWindow чтобы вывести ошибку о том
    // что создать категории не удалось
    bool isComplete = processCSV->createDataSet(dirPath);
    deleteNetworks();   // удаляем НС если они были созданы
    initNetworks();     // инициализация НС
    emit loadDataFinished(isComplete);
}

void MainProcess::trainingHidden()
{
    bool *cheksFinished = new bool[processCSV->getQuantityNetworks()] {false};
    for(int i = 0; i < processCSV->getQuantityNetworks(); i++)  // подаем сигналы на обучение
        emit startTrainingHiddenNetwork(i, processCSV->getFilePathTrain(), processCSV->getPosInBinFile().at(i), processCSV->getQuantityRowsRead().at(i), &(cheksFinished[i]));

    // ожидаем пока все НС ответят
    // будет крутится пока все НС не добавят по 1 к этому числу и оно не будет = их кол-ву
    while(true) {
        bool isFinished = true;
        for(int i = 0; i < processCSV->getQuantityNetworks(); i++) {
            if(cheksFinished[i] != true){
                isFinished = false;
                break;
            }
        }
        if(isFinished == true)
            break;
    }
    delete[] cheksFinished;
}

void MainProcess::initDataInFile()
{
    deleteNetworks();
    bool isInit = processCSV->initFromFile();
    if(isInit) // если файл нормально прочелся
    {
        mainNetwork = new MainNetwork();
        mainNetwork->open(processCSV->getDirPath());
        for(int i = 0; i < processCSV->getQuantityNetworks(); i++)
        {
            HiddenNetwork *newNetwork = new HiddenNetwork();
            newNetwork->setIndexThread(i);
            newNetwork->open(i, processCSV->getDirPath());
            QThread *newThread = new QThread(this);
            newNetwork->moveToThread(newThread);
            newThread->start();

            listNetworks.append(newNetwork);
            listThreads.append(newThread);

            // ДУБЛИРУЙ СИГНАЛЫ В INIT !!!!!!!!!!!!!!!!!!!

            // сигнал на завершение потока
            connect(this, SIGNAL(destroyed(QObject*)), newThread, SLOT(quit()));
            // сигнал на начало обучения НС
            connect(this, SIGNAL(startTrainingHiddenNetwork(int,QString,qint64,qint64,bool*)), newNetwork, SLOT(training(int,QString,qint64,qint64,bool*)));
            // сигнал на начало оценивания данных строки
            connect(mainNetwork, SIGNAL(startHiddenPredict(int, QList<double>,bool*)), newNetwork, SLOT(predicting(int, QList<double>,bool*)));
            // сохранение НС
            connect(this, SIGNAL(startSave(QString,int)), newNetwork, SLOT(saving(QString,int)));
            // получение настроек сети
            connect(this, SIGNAL(setSettingsNetworks(int,double,int)), newNetwork, SLOT(setSettingsNetworks(int,double,int)));
        }
        // передаем ссылку насписок НС первого слоя, чтобы иметь к ним прямой доступ (поток от этого не страдает)
        mainNetwork->setListHiddenNetwork(&listNetworks);
        // получение настроек сети
        connect(this, SIGNAL(setSettingsNetworks(int,double,int)), mainNetwork, SLOT(setSettingsNetworks(int,double,int)));
        emit initFinished(true);
        return;
    }
    emit initFinished(false);
}

void MainProcess::training()
{
    trainingHidden();   // обучаем первый слой
    emit trainingHidenFinished(true);
    mainNetwork->training(processCSV->getFilePathTrain());
    emit trainingMainFinished(true);
    processCSV->deleteTempFiles();
}

void MainProcess::predict(QString filePath)
{
    QList<QList<double>> listPredict; // ответы (вероятности ДА и НЕТ) построчно
    // создаем bin файл с нормализованными данными для предсказания (функция возвращает путь к созданному файлу)
    QString newfilePath = processCSV->createPredictDataSet(filePath);
    if(!newfilePath.isEmpty()) // если метод вернул путь к файлу
        listPredict = mainNetwork->prediction(newfilePath);
    qDebug() << listPredict;
    processCSV->deleteTempFiles();
}

void MainProcess::seving()
{
    processCSV->saveData();
    for(int i = 0; i < processCSV->getQuantityNetworks(); i++)
        emit startSave(processCSV->getDirPath(), i);    // отправляем путь и номер НС на сохранение
    mainNetwork->save(processCSV->getDirPath());
}
