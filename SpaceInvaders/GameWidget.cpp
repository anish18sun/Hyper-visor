#include "GameWidget.h"
#include <QIcon>
#include <QSound>
#include <QMovie>
#include <QPoint>
#include <QTimer>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QKeyEvent>
#include <QTimeLine>
#include <QMouseEvent>
#include <QEasingCurve>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

int ship3Counter = 10,                   //counter for the enemy ship to change the width of its appearance on screen
asteroidCounter = 0,                     //counter to control the appeearance of the asteroids on the screen
objectCounter = 0,                       //counter to control the appearance of other background objects
frameCounter = 0;                        //counter to control the appearance of the space frames on the screen
bool fireGun = false;
static int enemyPosX, enemyPosY;

GameWidget::GameWidget(QWidget *parent):QWidget(parent){

    //set basic parameters of the window widget
    setWindowIcon(QIcon(":/Game_Pictures/mainship.png"));
    setWindowTitle(tr("Space Invaders"));
    resize(1000,700);
    setCursor(Qt::CrossCursor);
    setFocusPolicy(Qt::StrongFocus);
    grabMouse();
    grabKeyboard();
    setMouseTracking(true);
    /*showFullScreen();*/

    //create the window background
    background = new QLabel(this);
    backgroundMovie = new QMovie(":/Game_Pictures/starsglow1.gif");
    background->setMovie(backgroundMovie);
    backgroundMovie->start();

    //create the sounds used for firing, explosions, launch sequence and the background Sound
    bulletSound = new QSound(":/Game_Sounds/mac10.wav",this);
    enemyExplosion = new QSound(":/Game_Sounds/explode5.wav",this);
    mainShipExplosion = new QSound(":/Game_Sounds/mortarhit.wav",this);
    launchSeq = new QSound(":/Game_Sounds/launch_seq.wav",this);
    backgroundSound = new QSound(":/Game_Sounds/thedocks_day1.wav",this);

    //create the background objects for the main window
    createBackgroundObjects();

    //create the main player ship
    mainShip = new MainShip(this);
    mainShip->move((width()/2)-65,height()-100);
    mainShip->show();

    //create the opening doors of the main game scene and open them into the game
    leftDoor = new QLabel(this);
    leftDoor->setPixmap(QPixmap(":/Game_Pictures/spaceDoorLeft1.png"));
    leftDoor->resize(800,700);
    leftDoor->move(0,0);

    rightDoor = new QLabel(this);
    rightDoor->setPixmap(QPixmap(":/Game_Pictures/spaceDoorRight1.png"));
    rightDoor->resize(1000,700);
    rightDoor->move(150,0);

    //create the enemy ships
    enemyShip1 = new EnemyShip(this,":/Game_Pictures/enemy1.png");
    enemyShip2 = new EnemyShip(this,":/Game_Pictures/enemy1.png");
    enemyShip3 = new EnemyShip(this,":/Game_Pictures/enemy1.png");
    megaEnemyShip1 = new EnemyShip(this,":/Game_Pictures/enemy2.1.png");
    megaEnemyShip2 = new EnemyShip(this,":/Game_Pictures/enemy2.2.png");

    //provide the visibility of the mainShip to the EnemyShip(s)
    EnemyShip::getMainShip(mainShip);

    //hide the ships initially so that the player may not know about them
    enemyShip1->hide();
    enemyShip2->hide();
    enemyShip3->hide();
    megaEnemyShip1->hide();
    megaEnemyShip2->hide();

    //create the explosion animtion for the main window widget
    explosion = new QLabel(this);
    explosionMovie = new QMovie(":/Game_Pictures/explosion1.gif");
    explosion->setMovie(explosionMovie);
    explosion->resize(250,180);
    explosionTime = new QTimeLine(3000,explosion);
    connect(explosionTime,SIGNAL(finished()),explosionMovie,SLOT(stop()));
    connect(explosionTime,SIGNAL(finished()),explosion,SLOT(hide()));    
    blast = new QLabel(this);
    blastMovie = new QMovie(":/Game_Pictures/blast1.gif");
    blast->setMovie(blastMovie);
    blast->resize(200,100);
    blastTime = new QTimeLine(500,blast);
    connect(blastTime,SIGNAL(finished()),blastMovie,SLOT(stop()));
    connect(blastTime,SIGNAL(finished()),blast,SLOT(hide()));
    createExplosion(enemyShip1);
    createExplosion(enemyShip2);
    createExplosion(enemyShip3);
    createExplosion(megaEnemyShip1);
    createExplosion(megaEnemyShip2);

    //set the animation timer for the ships that will go zig-zag down the screen
    timer1 = new QTimer();
    connect(timer1,SIGNAL(timeout()),enemyShip1,SLOT(refreshShip()));
    connect(timer1,SIGNAL(timeout()),enemyShip2,SLOT(refreshShip()));
    connect(timer1,SIGNAL(timeout()),enemyShip1,SLOT(show()));
    connect(timer1,SIGNAL(timeout()),enemyShip2,SLOT(show()));
    connect(timer1,SIGNAL(timeout()),this,SLOT(curveDown()));    

    //set the animation timer for the ship that will go straight down the screen
    timer2 = new QTimer();
    connect(timer2,SIGNAL(timeout()),enemyShip3,SLOT(refreshShip()));
    connect(timer2,SIGNAL(timeout()),enemyShip3,SLOT(show()));
    connect(timer2,SIGNAL(timeout()),this,SLOT(straightDown()));

    //set the animation timer for the ships that will spiral down the screen
    timer3 = new QTimer();
    connect(timer3,SIGNAL(timeout()),megaEnemyShip1,SLOT(refreshShip()));
    connect(timer3,SIGNAL(timeout()),megaEnemyShip2,SLOT(refreshShip()));
    connect(timer3,SIGNAL(timeout()),megaEnemyShip1,SLOT(show()));
    connect(timer3,SIGNAL(timeout()),megaEnemyShip2,SLOT(show()));
    connect(timer3,SIGNAL(timeout()),this,SLOT(spiralDown()));

    //create the background timer for the background objects
    backgroundTimer = new QTimer();
    connect(backgroundTimer,SIGNAL(timeout()),this,SLOT(animateBackground()));

    //create the bullet to be fired by the mainShip
    bullet = new Bullet(this);
    bullet->hide();
    fireBullet = new QPropertyAnimation(bullet,"geometry");
    fireBullet->setDuration(200);
    connect(fireBullet,SIGNAL(finished()),bullet,SLOT(hide()));

    //countdown the launch sequence for the launch of the ship
    QSound::play(":/Game_Sounds/Space_Invaders.wav");

    //set the game status to running mode
    status = gameRunning;               //start the game    
    launchDown();                       //launch the mainShip from the launchPad
}

//background objects creation module

void GameWidget::createBackgroundObjects(){
    //create the launch pad for the mainShip to launch
    launchPad = new QLabel(this);
    launchPad->setPixmap(QPixmap(":/Game_Pictures/launchPad1.png"));
    launchPad->resize(750,720);
    launchPad->move(width()/6,height()/7);

    //create the planets
    Jupiter = new QLabel(this);
    Jupiter->setPixmap(QPixmap(":/Game_Pictures/jupiter.png"));
    Jupiter->resize(800,800);
    Jupiter->hide();
    jupiterAnim = new QPropertyAnimation(Jupiter,"geometry");
    connect(jupiterAnim,SIGNAL(finished()),Jupiter,SLOT(hide()));
    jupiterAnim->setDuration(28000);
    jupiterAnim->setStartValue(QRect(-400,-800,800,800));
    jupiterAnim->setEndValue(QRect(-400,height()+800,800,800));

    Mars = new QLabel(this);
    Mars->setPixmap(QPixmap(":/Game_Pictures/Mars.png"));
    Mars->resize(500,500);
    Mars->hide();
    marsAnim = new QPropertyAnimation(Mars,"geometry");
    connect(marsAnim,SIGNAL(finished()),Mars,SLOT(hide()));
    marsAnim->setDuration(25000);
    marsAnim->setStartValue(QRect(width()-300,-500,500,500));
    marsAnim->setEndValue(QRect(width()-300,height()+500,500,500));

    //create the left background space frame and its corresponding animation
    leftFrame1 = new QLabel(this);
    leftFrame1->setPixmap(QPixmap(":/Game_Pictures/leftFrame1.png"));
    leftFrame1->resize(700,400);
    leftFrame1->hide();
    lframe1 = new QPropertyAnimation(leftFrame1,"geometry");
    connect(lframe1,SIGNAL(finished()),leftFrame1,SLOT(hide()));
    lframe1->setDuration(25000);
    lframe1->setStartValue(QRect(0,-500,700,400));
    lframe1->setEndValue(QRect(0,700,700,400));

    //create the right background space frame and its corresponding animation
    rightFrame1 = new QLabel(this);
    rightFrame1->setPixmap(QPixmap(":/Game_Pictures/rightFrame1.png"));
    rightFrame1->resize(700,700);
    rightFrame1->hide();
    rframe1 = new QPropertyAnimation(rightFrame1,"geometry");
    connect(rframe1,SIGNAL(finished()),rightFrame1,SLOT(hide()));
    rframe1->setDuration(28000);
    rframe1->setStartValue(QRect(width()-620,-700,700,700));
    rframe1->setEndValue(QRect(width()-620,height()+700,700,700));

    //create the asteroids
    asteroid1 = new QLabel(this);
    asteroid1->setPixmap(QPixmap(":/Game_Pictures/asteroid1.png"));
    asteroid1->resize(150,100);
    asteroid1->hide();
    asteroid1Anim = new QPropertyAnimation(asteroid1,"geometry");
    asteroid1Anim->setDuration(4000);

    asteroid2 = new QLabel(this);
    asteroid2->setPixmap(QPixmap(":/Game_Pictures/asteroid2.png"));
    asteroid2->resize(150,150);
    asteroid2->hide();
    asteroid2Anim = new QPropertyAnimation(asteroid2,"geometry");
    asteroid2Anim->setDuration(4000);

    //create the satellites
    satellite1 = new QLabel(this);
    sate1Movie = new QMovie(":/Game_Pictures/spaceObject1.gif");
    satellite1->setMovie(sate1Movie);
    satellite1->resize(150,150);
    sate1Anim = new QPropertyAnimation(satellite1,"geometry");
    connect(sate1Anim,SIGNAL(finished()),sate1Movie,SLOT(stop()));
    sate1Anim->setDuration(8000);

    satellite2 = new QLabel(this);
    sate2Movie = new QMovie(":/Game_Pictures/spaceObject2.gif");
    satellite2->setMovie(sate2Movie);
    satellite2->resize(150,150);
    sate2Anim = new QPropertyAnimation(satellite2,"geometry");
    connect(sate2Anim,SIGNAL(finished()),sate2Movie,SLOT(stop()));
    sate2Anim->setDuration(8000);

}

//event handling routines of this widget(main window):

void GameWidget::paintEvent(QPaintEvent *event){

    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.fillRect(rect(),Qt::black);
    background->resize(width(),height());
    backgroundMovie->setScaledSize(size());
}
void GameWidget::mouseMoveEvent(QMouseEvent *event){
    if(isWithinWindow(event->x(),event->y())) //check whether the coordinates are within the main window
        mainShip->move(event->pos());
    if((event->buttons() & Qt::LeftButton)&&(fireGun))
        fire(event->pos());
}
void GameWidget::mousePressEvent(QMouseEvent *event){
    if((status == gameRunning)&&(event->button() == Qt::LeftButton)){
        fireGun = true;
        EnemyShip::setGunStatus(fireGun);
        fire(event->pos());
    }
}
void GameWidget::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        fireGun = false; //shut the gun
        EnemyShip::setGunStatus(fireGun);
    }
}
void GameWidget::keyPressEvent(QKeyEvent *event){
    int x = mainShip->x(), y = mainShip->y(),
    key = event->key();
    //key events when the ship is to be moved
    if(key == Qt::Key_Left)
        x -= 5;
    else if(key == Qt::Key_Right)
        x += 5;
    else if(key == Qt::Key_Up)
        y -= 5;
    else if(key == Qt::Key_Down)
        y += 5;
    //check whether the coordinates are within the main game window
    if(isWithinWindow(x,y))
        mainShip->move(x,y);
    //key event to initiate firing from the ship
    if(key == Qt::Key_Space && status == gameRunning){
        fireGun = true;
        EnemyShip::setGunStatus(fireGun);
        fire(mainShip->pos());
    }
}
void GameWidget::keyReleaseEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Space){
        fireGun = false;                           //firing turned off
        EnemyShip::setGunStatus(fireGun);
   }
}

//coordinate checking module
bool GameWidget::isWithinWindow(int x,int y){
    if((x > -10)&&(x < width()-70) && (y > 0))
        return true;
    return false;
}

//animating modules of the main window widget:

void GameWidget::animateBackground(){
    //aniamte the leftFrame1 when the backgroundTimer emits timeout()
    leftFrame1->show();
    lframe1->start();

    //start the animation for the right frame
    if(frameCounter == 2){       //let the right space frame be dependent on the asteroidCounter
        rightFrame1->show();
        rframe1->start();
        frameCounter = 0;
    }

    //start the animation for the asteroids
    if(asteroidCounter == 1){
        asteroid1Anim->setStartValue(QRect(ship3Counter-10,-100,150,100));
        asteroid1Anim->setEndValue(QRect(mainShip->x(),height()+100,150,100));
        asteroid1->show();
        asteroid1Anim->start();
    }
    if(asteroidCounter == 2){
        asteroid2Anim->setStartValue(QRect(ship3Counter+10,-100,150,100));
        asteroid2Anim->setEndValue(QRect(mainShip->x(),height()+100,150,100));
        asteroid2->show();
        asteroid2Anim->start();
        asteroidCounter = 0;
    }

    //start the animation for the satellites
    if(objectCounter == 1){
        sate1Anim->setStartValue(QRect(ship3Counter-20,-150,150,150));
        sate1Anim->setEndValue(QRect(300,height()+150,150,150));
        satellite1->show();
        sate1Movie->start();
        sate1Anim->start();
    }
    if(objectCounter == 2){
        sate2Anim->setStartValue(QRect(ship3Counter+20,-150,150,150));
        sate2Anim->setEndValue(QRect(500,height()+150,150,150));
        satellite2->show();
        sate2Movie->start();
        sate2Anim->start();
    }

    //start the animation for the planets
    if(objectCounter == 3){
        Mars->show();
        marsAnim->start();
    }
    if(objectCounter == 4){
         Jupiter->show();
         jupiterAnim->start();
         objectCounter = 0;
    }

    //increment the counters that control the various animations
    frameCounter++;
    objectCounter++;
    asteroidCounter++;
}
void GameWidget::launchDown(){
    //open the doors of the main game window : SPACE INVADERS
    QPropertyAnimation *openLeftDoor = new QPropertyAnimation(leftDoor,"geometry");
    openLeftDoor->setDuration(10000);
    openLeftDoor->setStartValue(QRect(0,0,800,700));
    openLeftDoor->setKeyValueAt(0.2,QRect(0,0,800,700));
    openLeftDoor->setEndValue(QRect(-800,0,800,700));

    QPropertyAnimation *openRightDoor = new QPropertyAnimation(rightDoor,"geometry");
    openRightDoor->setDuration(10000);
    openRightDoor->setStartValue(QRect(150,0,1000,700));
    openRightDoor->setKeyValueAt(0.2,QRect(150,0,1000,700));
    openRightDoor->setEndValue(QRect(width(),0,1000,700));

    QParallelAnimationGroup *openDoor = new QParallelAnimationGroup();
    openDoor->addAnimation(openLeftDoor);
    openDoor->addAnimation(openRightDoor);

    //launch the ship from the launch Pad
    QPropertyAnimation *launch = new QPropertyAnimation(launchPad,"geometry");
    launch->setDuration(5000);
    launch->setStartValue(QRect(launchPad->x(),launchPad->y(),750,720));    
    launch->setEndValue(QRect(launchPad->x(),height(),750,720));        

    //connect all important signals/slots for the sequence of events to occur
    connect(openRightDoor,SIGNAL(finished()),launchSeq,SLOT(play()));
    connect(openRightDoor,SIGNAL(finished()),backgroundSound,SLOT(play()));
    connect(openDoor,SIGNAL(finished()),launch,SLOT(start()));
    connect(openDoor,SIGNAL(finished()),leftDoor,SLOT(deleteLater()));
    connect(openDoor,SIGNAL(finished()),rightDoor,SLOT(deleteLater()));
    connect(launch,SIGNAL(finished()),this,SLOT(startTimers()));
    connect(launch,SIGNAL(finished()),launchPad,SLOT(deleteLater()));

    openDoor->start(QAbstractAnimation::DeleteWhenStopped);              //start the game by opening the door
}
void GameWidget::curveDown(){ //slot for the ships going down the screen in a curved manner

    //define the animation for the first ship that will go zig-zag on the screen
    QPropertyAnimation *curveDown1 = new QPropertyAnimation(enemyShip1,"geometry");
    curveDown1->setDuration(6000);
    curveDown1->setKeyValueAt(0,QRect(50,0,100,100));
    curveDown1->setKeyValueAt(0.2,QRect(150,100,100,100));
    curveDown1->setKeyValueAt(0.4,QRect(50,200,100,100));
    curveDown1->setKeyValueAt(0.5,QRect(150,400,100,100));
    curveDown1->setEndValue(QRect(50,height(),100,100));
    curveDown1->setEasingCurve(QEasingCurve::OutBack); //easing curve to define the motion

    //define the animation for the second ship that will go zig-zag on the screen
    QPropertyAnimation *curveDown2 = new QPropertyAnimation(enemyShip2,"geometry");
    curveDown2->setDuration(6000);
    curveDown2->setKeyValueAt(0,QRect(550,0,100,100));
    curveDown2->setKeyValueAt(0.2,QRect(650,100,100,100));
    curveDown2->setKeyValueAt(0.4,QRect(550,200,100,100));
    curveDown2->setKeyValueAt(0.5,QRect(650,400,100,100));
    curveDown2->setEndValue(QRect(550,height(),100,100));
    curveDown2->setEasingCurve(QEasingCurve::OutBack); //easing curve to define the motion

    //set the zig-zag motion to occur parallel to each other as a parallel animation
    QParallelAnimationGroup *zigZag = new QParallelAnimationGroup();
    connect(zigZag,SIGNAL(finished()),enemyShip1,SLOT(hide()));
    connect(zigZag,SIGNAL(finished()),enemyShip2,SLOT(hide()));
    zigZag->addAnimation(curveDown1);
    zigZag->addAnimation(curveDown2);
    zigZag->start();    
}
void GameWidget::straightDown(){//slot for the ship going straight down

    //first reset the ship3Counter so that the counter for the ship3 does not exceed the width of the widget
    if(ship3Counter >= width())
        ship3Counter = 10; //set to the default value

    //define the animation for the ship going straight down
    QPropertyAnimation *straightDown1 = new QPropertyAnimation(enemyShip3,"geometry");
    straightDown1->setDuration(2000);
    straightDown1->setKeyValueAt(0,QRect(ship3Counter,0,100,100));
    straightDown1->setKeyValueAt(0.1,QRect(ship3Counter,100,100,100));
    straightDown1->setEndValue(QRect(ship3Counter,height(),100,100));
    straightDown1->setEasingCurve(QEasingCurve::InExpo);
    connect(straightDown1,SIGNAL(finished()),enemyShip3,SLOT(hide()));
    straightDown1->start();

    //increment the ship3Counter to change the width position at which the ship will occur on the screen
    ship3Counter += 200;
}
void GameWidget::spiralDown(){
    //animation for the megaEnemyShip1
    QPropertyAnimation *spiralDown1 = new QPropertyAnimation(megaEnemyShip1,"geometry");
    spiralDown1->setDuration(10000);
    spiralDown1->setStartValue(QRect(60,0,100,100));
    spiralDown1->setKeyValueAt(0.6,QRect(800,400,100,100));
    spiralDown1->setEndValue(QRect(60,height(),100,100));
    spiralDown1->setEasingCurve(QEasingCurve::OutSine);
    connect(spiralDown1,SIGNAL(finished()),megaEnemyShip1,SLOT(hide()));

    //animation for the megaEnemyShip2
    QPropertyAnimation *spiralDown2 = new QPropertyAnimation(megaEnemyShip2,"geometry");
    spiralDown2->setDuration(10000);
    spiralDown2->setStartValue(QRect(800,0,100,100));
    spiralDown2->setKeyValueAt(0.6,QRect(60,400,100,100));
    spiralDown2->setEndValue(QRect(800,height(),100,100));
    spiralDown2->setEasingCurve(QEasingCurve::OutSine);
    connect(spiralDown2,SIGNAL(finished()),megaEnemyShip2,SLOT(hide()));

    //create parallel animation for the two mega enemy ships
    QParallelAnimationGroup *spiral = new QParallelAnimationGroup();
    spiral->addAnimation(spiralDown1);
    spiral->addAnimation(spiralDown2);
    spiral->start();
}
void GameWidget::fire(QPoint position){
    if(fireGun){        
        fireBullet->setStartValue(QRect(position.x()+35,position.y()-30,30,30));
        fireBullet->setEndValue(QRect(position.x()+35,0,30,30));
        bullet->show();                
        fireBullet->start();
        if(bulletSound->isFinished())
            bulletSound->play();            //play the bullet sound only when it has finished playing once
    }
}//firing module over

//connection module for the explosions
void GameWidget::createExplosion(EnemyShip *enemyShip){
    // signal/slot connections for the explosion when the enemyShip hits the mainShip
    connect(enemyShip,SIGNAL(collision()),enemyShip,SLOT(hide()));
    connect(enemyShip,SIGNAL(collision()),mainShip,SLOT(hide()));    
    connect(enemyShip,SIGNAL(collision()),this,SLOT(switchGameState()));
    connect(enemyShip,SIGNAL(collision()),this,SLOT(startExplosion()));

    // signal/slot connections when the enemyShips are hit by the bullet    
    connect(enemyShip,SIGNAL(blast()),enemyShip,SLOT(hide()));
    connect(enemyShip,SIGNAL(blast()),this,SLOT(startBlast()));
}

//slots of the game :

void GameWidget::startTimers(){
    //start the timers for the animation
    timer1->start(11000);
    timer2->start(6000);
    timer3->start(15000);
    backgroundTimer->start(35000);
}
void GameWidget::startExplosion(){
    explosion->move(mainShip->x()-70,mainShip->y()-50);
    explosionTime->start();
    explosionMovie->start();
    explosion->show();
    mainShipExplosion->play();
}
void GameWidget::startBlast(){
    if(fireGun){
        EnemyShip::setEnemyPos(enemyPosX,enemyPosY);
        blast->move(enemyPosX-20, enemyPosY-20);
        blastTime->start();
        blastMovie->start();
        blast->show();
        if(enemyExplosion->isFinished())
            enemyExplosion->play();             //play the explosion sound only when the sound is stopped
    }
}
void GameWidget::switchGameState(){ //this slot switches the current state of the game
    if(status == gameRunning){
        gameOverLabel = new QLabel(this);
        gameOverLabel->setPixmap(QPixmap(":/Game_Pictures/game_over.png"));
        gameOverLabel->resize(600,300);
        gameOverLabel->move(200,240);
        gameOverLabel->show();        
        status = gameOver;
        fireGun = false;
        bullet->hide();
    }    
}
