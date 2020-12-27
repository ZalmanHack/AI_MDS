#include "readcsv.h"

ReadCSV::ReadCSV(QObject *parent) : QObject(parent)
{
    file = new QFile();
    streamFileRead = new QTextStream(file);
}

ReadCSV::~ReadCSV()
{
    file->close();
    delete file;
    delete streamFileRead;
}

QStringList ReadCSV::getTitles()
{
    return this->title;
}

bool ReadCSV::fileIsOpen()
{
    return isOpen;
}

bool ReadCSV::fileIsReadble()
{
    return !this->streamFileRead->atEnd();
}

QStringList ReadCSV::readRow() // не доделано
{
    QStringList result = streamFileRead->readLine().replace(",",".").replace(" ", "").split(";");
    if(!this->title.isEmpty() && this->title.size() != result.size())
        return QStringList();
    return result;
}

void ReadCSV::setStreamFileRead(QString fileName)
{
    file->setFileName(fileName);
}

bool ReadCSV::openFile(QString fileName) // new
{
    setStreamFileRead(fileName);
    try
    {
        if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) { // Проверка файла на существование
            throw (QString) "File is not be read";
        }
        streamFileRead->setCodec("UTF-8");                      // Задаем кодировку
        file->seek(0);
        QStringList list = readRow();
        if(!list.isEmpty()) {
            this->title = list;
            isOpen = true;                                // Чтение заголовка
        }
        else {
            isOpen = false;
        }
        return isOpen;
    }
    catch(QString str) {
        qDebug() << "ReadSVC :: " + str << endl;
        return false;
    }
}

void ReadCSV::closeFile()
{
    if(fileIsOpen()) {
        file->close();
        isOpen = false;
        title.clear();
    }
}

void ReadCSV::toBeginDataFile()
{
    streamFileRead->seek(0);
    streamFileRead->readLine();
}

void ReadCSV::toBeginFile()
{
    streamFileRead->seek(0);
}
