#ifndef GRAPHTRAIN_H
#define GRAPHTRAIN_H

#include <QWidget>
#include <QPainter>
#include <QTimer>

namespace Ui {
class GraphTrain;
}

class GraphTrain : public QWidget
{
    Q_OBJECT
private:
    QTimer timer;
    int graphic = 0; // точность сети в градусах на графике
    bool isWaitStarted = false;
    int step = 0;
    int chekForVisibleInfo = 0; // счетчик, который сработает и отобразит доп инфу при долгой подготовке

public:
    explicit GraphTrain(QWidget *parent = 0);
    ~GraphTrain();

    void startWaiting();
    void stopWaiting();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::GraphTrain *ui;

public slots:
    void timeOut();
    void setInfo(double errorTrain, int epoch);
};

#endif // GRAPHTRAIN_H
