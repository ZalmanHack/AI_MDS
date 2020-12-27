#include "graphtrain.h"
#include "ui_graphtrain.h"

GraphTrain::GraphTrain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphTrain)
{
    ui->setupUi(this);
    connect(&timer, SIGNAL(timeout()), this, SLOT(timeOut()));
}

GraphTrain::~GraphTrain()
{
    delete ui;
}

void GraphTrain::setInfo(double errorTrain, int epoch)
{
    chekForVisibleInfo = -1;
    ui->preparation->setVisible(false);
    ui->preparationInfo->setVisible(false);
    ui->accuracy->setVisible(true);
    ui->accuracy_label->setVisible(true);
    ui->epoch->setVisible(true);
    ui->epoch_label->setVisible(true);

    ui->epoch->setValue(epoch + 1);
    ui->accuracy->setValue(100.0 - errorTrain);
}

void GraphTrain::startWaiting()
{
    ui->preparation->setVisible(true);
    ui->preparationInfo->setVisible(false);
    ui->accuracy->setVisible(false);
    ui->accuracy_label->setVisible(false);
    ui->epoch->setVisible(false);
    ui->epoch_label->setVisible(false);
    chekForVisibleInfo = 0;

    timer.start(5);
}

void GraphTrain::stopWaiting()
{
    timer.stop();
}

void GraphTrain::paintEvent(QPaintEvent *event)
{
    // создаем рисовальщик для куска окружности
    QRectF rectangle(2.0, 2.0, 296.0, 296.0);
    int startAngle = 0;
    int spanAngle = 0;

    startAngle = step*16;
    spanAngle = 1440;

    QPainter painter(this);
    QPen pen(Qt::NoBrush, 4, Qt::SolidLine, 	Qt::RoundCap, Qt::RoundJoin);
    pen.setColor(QColor(234, 234, 234));
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawArc(rectangle, startAngle, spanAngle);
}

void GraphTrain::timeOut()
{
    if(chekForVisibleInfo >= 0) // при достижении 1000 он станет -1 чтобы лишний раз не считать. при завершении обучения станет 0
        chekForVisibleInfo++;
    if(chekForVisibleInfo == 1000) {
        ui->preparationInfo->setVisible(true);
        chekForVisibleInfo = -1;
    }
    if(step <= 360)
        step++;
    else
        step = 0;
    repaint();
}
