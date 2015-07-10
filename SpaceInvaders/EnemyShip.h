#ifndef ENEMYSHIP_H
#define ENEMYSHIP_H
#include <QWidget>
#include "MainShip.h"

class EnemyShip : public QWidget{
    Q_OBJECT
public :
    EnemyShip(QWidget *parent, QString);
    ~EnemyShip() {}
    static void getMainShip(MainShip *);
    static void setGunStatus(bool);
    static void setEnemyPos(int&, int&);

protected :
    void paintEvent(QPaintEvent *);    
    void moveEvent(QMoveEvent *);

private :
    QString face;                    //defines the appearance(face) of the enemyShip

    bool shipHit();                  //to check whether the EnemyShip is hit by bullets
    bool shipCollided();             //to check whether the EnemyShip is in the vicinity of collision with the mainShip    
    bool hasCollided, hasBeenHit;                //variable to check for collision

signals :
    void collision();
    void blast();

private slots :
    void refreshShip();
};
#endif // ENEMYSHIP_H
