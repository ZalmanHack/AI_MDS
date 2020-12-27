#ifndef PROCESSCSV_H
#define PROCESSCSV_H

#include <QObject>
#include <QDir>
#include <QDataStream>
#include <QDebug>
#include <assert.h>

#include "readcsv.h"
#include "normalize.h"

class ProcessCSV : public QObject
{
    Q_OBJECT

private:    
    QString dirDatasName = "TEMP AI";
    QString fileNormalizedDataTrain = "/data_train.bin";         // файл, куда сохраняются данные для обучения НС
    QString fileNormalizedDataPredict = "/data_predict.bin";     // файл, куда сохраняются данные для предсказания НС
    QString fileSaveData = "/processCSV_data.bin";                // файл, куда сохраняются данные класса

   // int quantityNetwork = 0;    // количество НС на первом слое
    struct Categories           // категория данных для НС
    {
        QList<QList<int>> indexTitles;  // индексы заголовков из titlesTextAll для текущей категории
        QStringList filesPath;          // индексы путей к файлам из filesPathAll для текщей категории
    };
    // категории заголовков. каждая категория = новая НС. в категории храняся индексы заголовков из QStringList allTitles;
    QList<Categories> categoriesAll;

    Normalize normalize;
    QStringList titlesTextAll;          // список всех названий заголовков
    QList<qint64> positionInBinFile;    // позиция начала чтения файла для каждой НС
    QList<qint64> quantityRowsForRead;  // количество строк, для каждой категории НС

    // загрука и распределение всех категорий
    void loadCategories(QString dirPath);
    // поиск титулов в уже известых
    QList<int> addTitlesInCategory(QStringList newTitles);
    // получение индексов для прочтенной строки которые соответствуют индексам полной (используется для пользовательских данных) (аналог addTitlesInCategory)
    QList<int> getIndexesFromTitles(QStringList newTitles);
    // поиск файла с такими же заголовками
    QList<int> findFileSomeTitles(QString filePath, QList<int> titles);  // проверка файла на схожесть заголовков
    // Создание файла с промежутками для нормализации
    void createNormalaizeData();
    void createTrainDataFile();



public:
    void                setTitlesTextAll(QStringList text);     // задание всех заголовков в текстовом формате

    QStringList         getTitlesTextAll();     // получение всех заголовков в текстовом формате
    QList<QList<int>>   getIndexesTitles();     // получение индексов заголовков для каждой НС
    int                 getQuantityNetworks();  // полченик кол-ва НС
    QList<int>          getQuantityInputs();    // кол-во входов для каждой НС
    //QList<double>       getMinRanges();         // полчение минимальных значений для заголовков НС
    //QList<double>       getMaxRanges();         // полчение максимальных значений для заголовков НС
    QList<qint64>       getPosInBinFile();      // полчение позиций для чтения данных в бин файле для каждой НС
    QList<qint64>       getQuantityRowsRead();  // кол-во строк, которое должна прочесть каждая НС, чтобы не выйти за свой промежуток данных
    int                 getQuantityMainInputs();// кол-во входов на самый первый слой (равно кол-ву заголовков)
    QString             getFilePathTrain();     // путь к файлу с подготовленными данными для обучения
    QString             getFilePathPredict();   // путь к файлу с подготовленными данными для предсказания
    void                deleteTempFiles();      // удаление всех временных файлов
    QString             getDirPath();           // путь к папке куда все сохраняется
    void                saveData();              // сохранение необходимых данных

    // нужно для выгрузки и чтения CAtegoryAll из и в файл
    QList<QList<QList<int>>> getCategoriesAllindexTitles();
    QList<QStringList>       getCategoriesAllFilesPath();
    void                     setCategoriesAll(QList<QList<QList<int>>> indexTitles, QList<QStringList> FilesPath);
    bool initFromFile();                        // инициализаия из файла

    explicit ProcessCSV(QObject *parent = nullptr);
    ~ProcessCSV();





signals:

public slots:
    bool createDataSet(QString dirPath);    // создание тренеровачного датасета
    QString createPredictDataSet(QString filePath); // создание пользовательского датасета (для предсказания)
};

#endif // PROCESSCSV_H
