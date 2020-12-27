#ifndef READCSV_H
#define READCSV_H

#include <QObject>
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <QStringList>

class ReadCSV : public QObject
{
    Q_OBJECT

private:

    QStringList title;
    bool isOpen = false;
    QTextStream *streamFileRead;
    QFile *file;

public:
    ReadCSV(QObject *parent = nullptr);
    ~ReadCSV();
    QStringList getTitles();                                                    // Получение заголовка таблицы без выборки

    bool fileIsOpen();
    bool fileIsReadble();

    QStringList readRow();                               // Чтение строки или заголовка  +

    void setStreamFileRead(QString fileName);
    bool openFile(QString fileName);
    void closeFile();
    void toBeginDataFile(); // переместиться на начало данных в файле
    void toBeginFile();     // переместиться на начало файла (вместе с заголовком)



signals:

public slots:
};

#endif // READCSV_H
