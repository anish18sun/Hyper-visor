#include "Bullet.h"
#include <QPainter>
#include <QImage>
#include <QPen>

Bullet::Bullet(QWidget *parent) : QWidget(parent){
    resize(100,100); //this bullet shaped to the size of the bullet
}
void Bullet::paintEvent(QPaintEvent *event){
    QWidget::paintEvent(event);
    //set up the bullet painter
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    QPen pen(Qt::NoPen);
    painter.setPen(pen);
    painter.drawImage(rect(),QImage(":/Game_Pictures/bullets.png"));
} //bullet painted (face of the bullet widget created)
