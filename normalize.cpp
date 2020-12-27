#include "normalize.h"

Normalize::Normalize(QObject *parent) : QObject(parent)
{

}

QList<double> Normalize::toDouble(QStringList row)
{
    QList<double> result;
    for(QString item : row)
        result.append(item.toDouble());
    return result;
}

QList<double> Normalize::toDouble(QString* row, int size)
{
    QList<double> result;
    for(int i = 0; i < size; i++)
        result.append(row[i].toDouble());
    return result;
}

QList<double> Normalize::toDouble(double *row, int size)
{
    QList<double> result;
    for(int i = 0; i < size; i++)
        result.append(row[i]);
    return result;
}

void Normalize::addRowVariable(QList<double> row)
{
    try {
        if(row.isEmpty())
            throw (QString) "row size is empty";
        if(sizelistNormalize == 0) {
            listNormalizeVariable = new minMaxVariable[row.size()];
            sizelistNormalize = row.size();
        }
        if(row.size() != sizelistNormalize)
            throw (QString) QString::number(row.size()) + " - " + QString::number(sizelistNormalize) + ":: row size != listNormalizeVariable size";

        for(int i = 0; i < row.size(); i++)             // сравнение строк и поиск максимума
        {
            if(row.at(i) >= listNormalizeVariable[i].max)
                listNormalizeVariable[i].max = row.at(i);
            else if(row.at(i) < listNormalizeVariable[i].min)
                listNormalizeVariable[i].min = row.at(i);
        }
    }
    catch(QString str) {
        qDebug() << "Network ::" << str << endl;
    }
}

void Normalize::addRowVariable(double *row, int size)
{
    try {
        if(size == 0)
            throw (QString) "row size is empty";
        if(sizelistNormalize == 0) {
            listNormalizeVariable = new minMaxVariable[size];
            sizelistNormalize = size;
        }
        if(size != sizelistNormalize)
            throw (QString) QString::number(size) + " - " + QString::number(sizelistNormalize) + ":: row size != listNormalizeVariable size";
        for(int i = 0; i < size; i++)             // сравнение строк и поиск максимума
        {
            if(row[i] > listNormalizeVariable[i].max)
                listNormalizeVariable[i].max = row[i];
            else if(row[i] < listNormalizeVariable[i].min)
                listNormalizeVariable[i].min = row[i];
        }
    }
    catch(QString str) {
        qDebug() << "Network ::" << str << endl;
    }
}


QList<double> Normalize::getNormalize(QList<double> row)
{
    QList<double> result;
    try {
        if(sizelistNormalize < 1)
            throw (QString) "sizelistNormalize size <= 0";
        if(row.size() != sizelistNormalize)
            throw (QString) "Row size != sizelistNormalize size";
        for(int i = 0; i < row.size(); i++)
            if(row.at(i) == 0)
                result.append(0);
            else
                result.append( (row.at(i) - listNormalizeVariable->min)/(listNormalizeVariable[i].max - listNormalizeVariable->min) ); // формула нормализации (X - MIN)/(MAX - MIN)
        return result;
    }
    catch(QString str) {
        qDebug() << "Network ::" << str << endl;
        return result;
    }
}

QList<double> Normalize::getNormalize(double *row, int size)
{
    QList<double> result;
    try {
        if(sizelistNormalize < 1)
            throw (QString) "sizelistNormalize size <= 0";
        if(size != sizelistNormalize)
            throw (QString) "Row size != sizelistNormalize size";
        for(int i = 0; i < size; i++)
            if(row[i] == 0)
                result.append(0);
            else
                result.append( (row[i] - listNormalizeVariable->min)/(listNormalizeVariable[i].max - listNormalizeVariable->min) ); // формула нормализации (X - MIN)/(MAX - MIN)
        return result;
    }
    catch(QString str) {
        qDebug() << "Network ::" << str << endl;
        return result;
    }
}

QList<double> Normalize::getMinVariable()
{
   QList<double> result;
    for(int i = 0; i < sizelistNormalize; i++)
        result.append(listNormalizeVariable[i].min);
    return result;
}

QList<double> Normalize::getMaxVariable()
{
   QList<double> result;
    for(int i = 0; i < sizelistNormalize; i++)
        result.append(listNormalizeVariable[i].max);
    return result;
}

void Normalize::setMinMaxVariable(QList<double> min, QList<double> max)
{
    assert(min.size() == min.size());
    if(listNormalizeVariable == nullptr || sizelistNormalize == 0) {
        listNormalizeVariable = new minMaxVariable[min.size()];
        sizelistNormalize = min.size();
    }
    for(int i = 0; i < min.size(); i++) {
        listNormalizeVariable[i].min = min.at(i);
        listNormalizeVariable[i].max = max.at(i);
    }
}

void Normalize::clear()
{
    sizelistNormalize = 0;
    delete[] listNormalizeVariable;
    listNormalizeVariable = nullptr;
}
