#ifndef BULLET_H
#define BULLET_H
#include <QWidget>
class Bullet : public QWidget{
    Q_OBJECT
public:
    Bullet(QWidget *parent = 0);
    ~Bullet() {}
protected :
    void paintEvent(QPaintEvent *);
};

#endif // BULLET_H
