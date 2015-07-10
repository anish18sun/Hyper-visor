#include "MainShip.h"
#include <QPainter>
#include <QPen>
#include <QImage>

MainShip::MainShip(QWidget *parent):QWidget(parent){
    resize(100,100);
}
void MainShip::paintEvent(QPaintEvent *event){

    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    QPen pen(Qt::NoPen);
    painter.setPen(pen);    
    painter.drawImage(rect(),QImage(":/Game_Pictures/mainship.png"));
}

