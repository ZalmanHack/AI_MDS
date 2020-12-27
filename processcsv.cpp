#include "processcsv.h"

ProcessCSV::ProcessCSV(QObject *parent) : QObject(parent)
{

}

ProcessCSV::~ProcessCSV()
{

}

void ProcessCSV::setTitlesTextAll(QStringList text)
{
    this->titlesTextAll = text;
}

QStringList ProcessCSV::getTitlesTextAll()
{
    return this->titlesTextAll;
}

QList<QList<int> > ProcessCSV::getIndexesTitles()
{
    QList<QList<int> > result;
    //assert(!categoriesAll.isEmpty());
    for(Categories item : categoriesAll) {
        result.append(item.indexTitles.at(0));
        std::sort(result.back().begin(), result.back().end());
    }
    return result;
}

int ProcessCSV::getQuantityNetworks()
{
    return this->categoriesAll.size();
}

QList<int> ProcessCSV::getQuantityInputs()
{
    QList<int> result;
    for(QList<int> item : getIndexesTitles())
        result.append(item.size());
    return result;
}

QList<qint64> ProcessCSV::getPosInBinFile()
{
    return this->positionInBinFile;
}

QList<qint64> ProcessCSV::getQuantityRowsRead()
{
    return this->quantityRowsForRead;
}

int ProcessCSV::getQuantityMainInputs()
{
    return this->titlesTextAll.size();
}

QString ProcessCSV::getFilePathTrain()
{
    return this->dirDatasName + this->fileNormalizedDataTrain;
}

QString ProcessCSV::getDirPath()
{
    return this->dirDatasName;
}

QString ProcessCSV::getFilePathPredict()
{
    return this->dirDatasName + this->fileNormalizedDataPredict;
}

void ProcessCSV::deleteTempFiles()
{
    // создаем папку, где будет храниться файл, с нормализированными данными для обучения
    if(!QDir(this->dirDatasName).exists())
        QDir().mkdir(this->dirDatasName);

    QFile delFile(this->dirDatasName + fileNormalizedDataTrain);
    if(delFile.exists())
        delFile.remove();
    delFile.setFileName(this->dirDatasName + fileNormalizedDataPredict);
    if(delFile.exists())
        delFile.remove();
}

void ProcessCSV::saveData()
{
    // создаем папку, где будет храниться файл, с нормализированными данными для обучения
    if(!QDir(this->dirDatasName).exists())
        QDir().mkdir(this->dirDatasName);

    // читаем исходные файлы и записываем их в файл с нормализацией
    QFile file(this->dirDatasName + this->fileSaveData);
    QDataStream stream(&file);
    stream.setVersion(QDataStream::Qt_5_10);
    if(file.open(QIODevice::WriteOnly))
    {
        stream << getTitlesTextAll();   // сохраняем названия заголовков
        stream << normalize.getMinVariable();       // сохраняем минимальные значения столбцов
        stream << normalize.getMaxVariable();       // сохраняем максимальные значения столбцов
        stream << getCategoriesAllindexTitles();    // выгружаем индексы из CategoryAll
        stream << getCategoriesAllFilesPath();      // выгружаем пути к файлам из CategoryAll
        file.close();
    }
}

QList<QList<QList<int>>> ProcessCSV::getCategoriesAllindexTitles()
{
    QList<QList<QList<int>>> result;
    for(Categories category : categoriesAll)
        result.append(category.indexTitles);
    return result;
}

QList<QStringList> ProcessCSV::getCategoriesAllFilesPath()
{
    QList<QStringList> result;
     for(Categories category : categoriesAll)
         result.append(category.filesPath);
     return result;
}

void ProcessCSV::setCategoriesAll(QList<QList<QList<int> > > indexTitles, QList<QStringList> FilesPath)
{
    assert(indexTitles.size() == FilesPath.size());
    categoriesAll.clear();
    for(int i = 0; i <  FilesPath.size(); i++)
    {
        Categories temp;
        temp.filesPath.append(FilesPath.at(i));
        temp.indexTitles.append(indexTitles.at(i));
        categoriesAll.append(temp);
    }

}


bool ProcessCSV::initFromFile()
{
    // создаем папку, где будет храниться файл, с нормализированными данными для обучения
    if(!QDir(this->dirDatasName).exists())
        QDir().mkdir(this->dirDatasName);


    // читаем исходные файлы и записываем их в файл с нормализацией
    QFile file(this->dirDatasName + this->fileSaveData);

    qDebug() << file.exists();

    if(file.exists() && file.open(QIODevice::ReadOnly))
    {
        QDataStream stream(&file);
        stream.setVersion(QDataStream::Qt_5_10);
        QStringList strList;
        QList<double> min, max;
        QList<QList<QList<int>>> indexes;
        QList<QStringList> filesPath;
        stream >> strList;
        setTitlesTextAll(strList);  // сохраняем названия заголовков
        stream >> min;
        stream >> max;
        normalize.setMinMaxVariable(min, max);       // сохраняем минимальные значения столбцов
        stream >> indexes;
        stream >> filesPath;
        setCategoriesAll(indexes, filesPath);    // загружаем индексы из CategoryAll
        file.close();
        return true;
    }
    return false;
}

void ProcessCSV::loadCategories(QString dirPath)
{
    titlesTextAll.clear();
    categoriesAll.clear();

    ReadCSV rCSV;
    QDir dir(dirPath);
    QFileInfoList dirContent = dir.entryInfoList(QStringList()
    << "*.csv", QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);    // Получаем названия всех .CSV файлов из паки data
    for(int i = 0; i < dirContent.size(); i++) {                     // перебор всех файлов
        rCSV.openFile(dirContent.at(i).filePath());
        if(!rCSV.getTitles().isEmpty()) {
            // --------------------------------------------------------------- формируем категорию
            Categories newCategory;                                         // новая категория
            QStringList newTitles = rCSV.getTitles();                       // новые прочитанные титулы
            newCategory.indexTitles.append(addTitlesInCategory(newTitles)); // записываем индексы добавленных в общий массив заголовков
            newCategory.filesPath.append(dirContent.at(i).filePath());      // добавляем текущий путь файла
            // --------------------------------------------------------------- поиск и добавление файлов с таким же заголовками в категорию
            for(int j = i+1; j < dirContent.size(); j++) {
                //передаем путь к файлу и нужные заголовки в функцию поиска, если файл по этому пути содержет такие заголовки, то метод вернет их , а иначе пустоту
                QList<int> newIndexTitles = findFileSomeTitles(dirContent.at(j).filePath(), newCategory.indexTitles.back());
                if(!newIndexTitles.isEmpty()) {                                 // если ответ функции не пустота
                    newCategory.indexTitles.append(newIndexTitles);             // добавляем заголовок в категорию
                    newCategory.filesPath.append(dirContent.at(j).filePath());  // добавляем путь к файлу в категорию
                    dirContent.removeAt(j);                                     // удаляем путь к файлу
                    j--;                                                        // уменьшаем j, чтобы не пропустить следующий путь
                }
            }
            categoriesAll.append(newCategory);
            qDebug() << "-------------" << i+1 << "-------------";
            //qDebug() << categoriesAll.back().filesPath;
            //qDebug() << categoriesAll.back().indexTitles;
        }
        rCSV.closeFile();
    }
    dirContent.clear();
}

QList<int> ProcessCSV::addTitlesInCategory(QStringList newTitles)
{
    QList<int> result;  // выходной массив с индексами расположения новых заголовков внутри в известных
    for(int newItem = 0; newItem < newTitles.size(); newItem++)  { // ищем текущие заголовки в уже известных
        bool isFound = false;
        for(int item = 0; item < titlesTextAll.size(); item++){
            if(QString::compare(titlesTextAll.at(item), newTitles.at(newItem)) == 0) {
                isFound = true;
                result.append(item); // записываем индекс расположения заголовка (колнки) внутри массива всех заголовков
                break;
            }
        }
        if(!isFound) {
            titlesTextAll.append(newTitles.at(newItem));  // если не нашли то добавляем и записываем индекс
            result.append(titlesTextAll.size() - 1);
        }
    }
    return result;
}

QList<int> ProcessCSV::getIndexesFromTitles(QStringList newTitles) // получение индексов для прочтенной строки которые соответствуют индексам полной
{
    QList<int> result;  // выходной массив с индексами расположения новых заголовков внутри в известных
    for(int newItem = 0; newItem < newTitles.size(); newItem++)  { // ищем текущие заголовки в уже известных
        bool isFound = false;
        for(int item = 0; item < titlesTextAll.size(); item++){
            if(QString::compare(titlesTextAll.at(item), newTitles.at(newItem)) == 0) {
                isFound = true;
                result.append(item); // записываем индекс расположения заголовка (колнки) внутри массива всех заголовков
                break;
            }
        }
        if(!isFound)
            result.append(titlesTextAll.size() - 1);
    }
    return result;
}

QList<int> ProcessCSV::findFileSomeTitles(QString filePath, QList<int> titles)
{
    QList<int> result;
    ReadCSV tempReadCSV;                                    // создаем класс для чтения строк
    tempReadCSV.openFile(filePath);                         // открываем файл
    if(tempReadCSV.fileIsOpen()) {                          // если файл открылся
        if(tempReadCSV.getTitles().isEmpty()) {             // если заголовок в файле пуст
            tempReadCSV.closeFile();                        // закрываем файл
            return result;
        }
        QList<int> newTitles = addTitlesInCategory(tempReadCSV.getTitles());  // получаем индексы заголовков прочтенного файла
        tempReadCSV.closeFile();
        bool isSome = false;                                // проверка на схожеть заголовков
        for(int item : titles) {                            // перебор нужных заголовков
            isSome = false;                                 // ставим лож, чтобы проверить каждый столбец
            for(int newItem : newTitles)                    // перебираем новые заголовки
                if(item == newItem) {                       // сравниваем их значение (там индексы)
                    isSome = true;                          // если текущие совпали то ставим истину
                    break;                                  // переходим к следующему заголовку
                }
            if(!isSome)                                     // если не совпал хотябы 1, то выходим из цикла
                break;
        }
        if(isSome)                                          // если все совпало, то возвращаем их
            result = newTitles;
    }
    return result;
}

void ProcessCSV::createNormalaizeData()
{
    ReadCSV reader;
    normalize.clear();                                       // очищаем нормализацию данных
    for(Categories category : categoriesAll) {               // перебираем категории, в которых есть пути файлов и номер столбцов
        for(int i = 0; i < category.filesPath.size(); i++) { // перебираем все файлы из данной категории
            if(reader.openFile(category.filesPath.at(i))) {  // открыли один из файлов, который хранился в одной из выбранной категории
                while(reader.fileIsReadble())                // читаем файл построчно
                {
                    QList<double> readingRow = normalize.toDouble(reader.readRow()); // прочтенная строка из файла
                    double *tempRow = new double[titlesTextAll.size()] {0};              // создаем строку, размером со все заголовки, для бинарного файла
                    for(int j = 0; j < category.indexTitles.at(i).size(); j++)       // записываем прочтенную строку в строго определенные ячейки
                        tempRow[category.indexTitles.at(i).at(j)] = readingRow.at(j);
                    normalize.addRowVariable(tempRow, titlesTextAll.size());            // добавляем данные для нормализации
                    delete[] tempRow;                                               // удаляем временную строку
                }
                reader.closeFile();
            }
        }
    }
}

void ProcessCSV::createTrainDataFile()
{
    positionInBinFile.clear();
    quantityRowsForRead.clear();

    ReadCSV reader;
    // создаем папку, где будет храниться файл, с нормализированными данными для обучения
    if(!QDir(this->dirDatasName).exists())
        QDir().mkdir(this->dirDatasName);

    // читаем исходные файлы и записываем их в файл с нормализацией
    QFile file(this->dirDatasName + this->fileNormalizedDataTrain);
    QDataStream stream(&file);
    stream.setVersion(QDataStream::Qt_5_10);

    QList<double> expectedPredict;
    expectedPredict << 0 << 1;

    if(file.open(QIODevice::WriteOnly)) {
        qint64 sizeRows = 0;                                                                            // обнуляем кол-во строк для данной категории
        for(Categories category : categoriesAll)  {                                                     // перебираем категории, в которых есть пути файлов и номер столбцов
            positionInBinFile.append(file.pos());                                                       // запоминаем начальную позицию в файле для перемещения на нужную строку НС
            for(int i = 0; i < category.filesPath.size(); i++) {                                        // перебираем все файлы из данной категории
                if(reader.openFile(category.filesPath.at(i))) {                                         // открыли один из файлов, который хранился в одной из категории
                    while(reader.fileIsReadble())                                                       // читаем файл построчно
                    {
                        QList<double> readingRow = normalize.toDouble(reader.readRow());                // прочтенная строка из файла
                        double *tempRow = new double[titlesTextAll.size()] {0};                             // создаем строку, размером со все заголовки, для бинарного файла
                        for(int j = 0; j < category.indexTitles.at(i).size(); j++)                      // записываем прочтенную строку в строго определенные ячейки
                            tempRow[category.indexTitles.at(i).at(j)] = readingRow.at(j);
                        stream << normalize.getNormalize(normalize.toDouble(tempRow, titlesTextAll.size()));// получаем нормализованное значение и записываем его
                        if(category.filesPath.at(i).split("_").back().split(".").first() == "0")          // если файл был с ложными данными, то пишем рядом со строкой FALSE
                            stream << expectedPredict[0];
                        else if(category.filesPath.at(i).split("_").back().split(".").first() == "1")     // если файл был с ложными данными, то пишем рядом со строкой TRUE
                            stream << expectedPredict[1];
                        sizeRows++;                                                                     // плюсуем кол-во строк для данной категории
                        delete[] tempRow;
                    }
                    reader.closeFile();
                }
            }
            quantityRowsForRead.append(sizeRows);                                                               // запоминамем колво строк для категории
            sizeRows = 0;                                                                               // обнуляем кол-во строк для данной категории                                                          // добавляем мета данные для категории
        }
        file.close();
    }
}

bool ProcessCSV::createDataSet(QString dirPath)
{
    // инициализируем массив категорий
    loadCategories(dirPath);

    // создаем массивы с промужутками min max для каждой НС
    createNormalaizeData();

    // создамем файл со всеми данными, у которого длина строки == кол-ву всех столбцов
    // а также метод возвращает мета данные для ориентерования по файлу
    createTrainDataFile();

    {
        qDebug() << "titlesAll";
        //qDebug() << titlesTextAll;
        qDebug() << "Мин:" << normalize.getMinVariable();
        qDebug() << "Макс:" << normalize.getMaxVariable();
        qDebug() << "POS IN FILE";
        qDebug() << getPosInBinFile();
        qDebug() << getQuantityRowsRead();

    }
    return !categoriesAll.isEmpty(); // если ни одной категории небыло создано то возвращаем false иначе true;
}

QString ProcessCSV::createPredictDataSet(QString filePath)
{
    ReadCSV reader; // читает csv файл пользователя
    QFile file(this->dirDatasName + this->fileNormalizedDataPredict);
    QDataStream stream(&file);  // пишет в бинарный файл нормализованные данные
    stream.setVersion(QDataStream::Qt_5_10);

    // создаем папку если ее нет (но она должна быть, это все на всякий случай), где будет храниться файл, с нормализированными данными для обучения
    if(!QDir(this->dirDatasName).exists())
        QDir().mkdir(this->dirDatasName);

    bool isRowWrite = false;  // будет true если в bin файл будет записана хотябы одна строка

    if(file.open(QIODevice::WriteOnly) && reader.openFile(filePath)) {
        if(!reader.getTitles().isEmpty()) {
            QList<int> indexes = getIndexesFromTitles(reader.getTitles());       // получаем индексы заголовков для нашего файла
            while(reader.fileIsReadble())                                        // читаем файл построчно
            {
                isRowWrite = true;                                               // в файл записана хотябы одна строка, это значит, что функция уже удачная
                QList<double> readingRow = normalize.toDouble(reader.readRow()); // прочтенная строка из файла
                double *tempRow = new double[titlesTextAll.size()] {0};          // создаем строку, размером со все заголовки, для бинарного файла
                for(int j = 0; j < indexes.size(); j++)                          // записываем прочтенную строку в строго определенные ячейки
                    tempRow[indexes.at(j)] = readingRow.at(j);
                stream << normalize.getNormalize(tempRow, titlesTextAll.size()); // нормализуем данные и записываем в bib файл
                delete[] tempRow;                                                // удаляем временную строку
            }
            file.close();
        }
        reader.closeFile();
    }
    if(isRowWrite)
        return this->dirDatasName + this->fileNormalizedDataPredict;  // возвращаем результат выполнения
    else
        return "";
}
