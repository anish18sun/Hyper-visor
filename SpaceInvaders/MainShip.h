#ifndef MAINSHIP_H
#define MAINSHIP_H

#include <QWidget>

class MainShip : public QWidget{
    Q_OBJECT
public :
     MainShip(QWidget *parent = 0);
    ~MainShip() {}

protected :
    virtual void paintEvent(QPaintEvent *);
    virtual QSize sizeHint() const {return QSize(200,200);}    
};

#endif // MAINSHIP_H
