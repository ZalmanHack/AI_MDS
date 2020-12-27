#ifndef NORMALIZE_H
#define NORMALIZE_H

#include <QObject>
#include <QDebug>
#include <assert.h>

class Normalize : public QObject
{
    Q_OBJECT
private:
    struct minMaxVariable {
        double min = 0;
        double max = 0;
    };
    int sizelistNormalize = 0;
    minMaxVariable* listNormalizeVariable = nullptr; // список мин и макс значений для нормализации по каждому столбцу

public:
    explicit Normalize(QObject *parent = nullptr);
    QList<double> toDouble(QStringList row);        // конвертация в double
    QList<double> toDouble(QString* row, int size);
    QList<double> toDouble(double* row, int size);
    void          addRowVariable(QList<double> row);         // добавление данных для поиска мин макс значений для нормализации
    void          addRowVariable(double *row, int size);     // добавление данных для поиска мин макс значений для нормализации
    QList<double> getNormalize(QList<double> row);  // нормализировать входную строку
    QList<double> getNormalize(double* row, int size);  // нормализировать входную строку

    QList<double> getMinVariable();                     // возвращает минимальные промжетуки для столбцов
    QList<double> getMaxVariable();                     // возвращает максимальные промжетуки для столбцов

    void          setMinMaxVariable(QList<double> min, QList<double> max);

    void clear();

signals:

public slots:
};

#endif // NORMALIZE_H
