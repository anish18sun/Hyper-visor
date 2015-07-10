#include "EnemyShip.h"
#include <QPainter>
#include <QPen>
#include <QImage>

static bool gunStatus = false;
static int xPos, yPos;
static MainShip *ship;

EnemyShip::EnemyShip(QWidget *parent, QString faceFile) : QWidget(parent){
    face = faceFile;
    resize(100,100);
    hasBeenHit = false;
    hasCollided = false;    
}
void EnemyShip::paintEvent(QPaintEvent *event){
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    QPen pen(Qt::NoPen);
    painter.setPen(pen);
    painter.drawImage(rect(),QImage(face));
}
void EnemyShip::moveEvent(QMoveEvent *){
    if((!hasBeenHit) && shipHit() && (gunStatus)){
        xPos = x();
        yPos = y();
        emit blast();
        hasBeenHit = true;
    }
    else if((!hasBeenHit) && (!hasCollided) && shipCollided()){
        emit collision();
        hasCollided = true;
    }
}
bool EnemyShip::shipHit(){    
    if((x() >= ship->x()-20) && (x() <= ship->x()+20))
        return true;
    return false;
}
bool EnemyShip::shipCollided(){
    if(((y() >= ship->y()-25)&&(y() <= ship->y()+25)) && ((x() >= ship->x()-30)&&(x() <= ship->x()+30))){
        return true;
    }
        return false;
}
void EnemyShip::refreshShip(){
    hasBeenHit = false;
}
void EnemyShip::getMainShip(MainShip *mainShip){
    ship = mainShip;
}
void EnemyShip::setGunStatus(bool status){
    gunStatus = status;
}
void EnemyShip::setEnemyPos(int &x, int &y){
    x = xPos;
    y = yPos;
}
