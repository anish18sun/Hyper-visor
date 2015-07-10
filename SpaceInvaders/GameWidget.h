#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H
#include "MainShip.h"
#include "EnemyShip.h"
#include "Bullet.h"
#include <QWidget>

class QMovie;
class QSound;
class QPoint;
class QTimer;
class QLabel;
class QTimeLine;
class QPropertyAnimation;

class GameWidget : public QWidget{
    Q_OBJECT

public :
    GameWidget(QWidget *parent = 0);
    ~GameWidget() {}
    enum GameStatus{gameOver=0, gameRunning=1};

protected :
    virtual void paintEvent(QPaintEvent *);
    virtual QSize sizeHint() const {return QSize(200,200);}
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    void fire(QPoint);

private:

    GameStatus status;                                  //enumeration to hold the status of the game

    //various Qtproperties of the game :

    QLabel *background, *explosion, *blast, *launchPad, *leftDoor, *rightDoor, *leftFrame1,
    *asteroid1, *asteroid2, *rightFrame1, *satellite1, *satellite2, *Jupiter, *Mars, *gameOverLabel;

    QMovie *backgroundMovie, *explosionMovie, *blastMovie, *sate1Movie, *sate2Movie;
    QSound *bulletSound, *enemyExplosion, *mainShipExplosion, *launchSeq, *backgroundSound;

    MainShip *mainShip;
    EnemyShip *enemyShip1,*enemyShip2,*enemyShip3, *megaEnemyShip1, *megaEnemyShip2;
    QTimer *timer1,*timer2, *timer3, *backgroundTimer;
    Bullet *bullet;

    QPropertyAnimation *fireBullet, *lframe1, *asteroid1Anim, *asteroid2Anim, *rframe1,
    *sate1Anim, *sate2Anim, *jupiterAnim, *marsAnim;

    QTimeLine *explosionTime, *blastTime;                 //timeLine for the explosion animation

    void createExplosion(EnemyShip *);
    void createBackgroundObjects();
    void launchDown();
    bool isWithinWindow(int,int);             //module to check whether the ship is within window

protected slots :
    void curveDown();                         //to animate the EnemyShips that curve down the screen
    void spiralDown();
    void straightDown();    
    void startExplosion();
    void startBlast();
    void startTimers();
    void animateBackground();                 //to animate the background objects
    void switchGameState();                   //used to switch the current game state

};

#endif // GAMEWIDGET_H
