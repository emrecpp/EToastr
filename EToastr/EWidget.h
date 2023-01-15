#ifndef EWIDGET_H
#define EWIDGET_H

#include <QWidget>
#include <QMainWindow>
#include <QResizeEvent>
#include <QMoveEvent>

class EWidget : public QMainWindow
{
    Q_OBJECT
public:
    explicit EWidget(QMainWindow *parent = nullptr);
signals:
    void resizeEvent(QResizeEvent *event);
    void moveEvent(QMoveEvent *event);
};
#endif // EWIDGET_H
