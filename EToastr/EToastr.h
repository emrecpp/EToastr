// Emre Demircan
// Github: github.com/emrecpp
// Version: 1.0.0
// Date: 2023-01-01
// Thanks to Github/MilkyWay7845

#ifndef ETOASTR_H
#define ETOASTR_H

#include "EWidget.h"
#include <QWidget>
#include <QFrame>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QTime>
#include <QTimer>
#include <QLabel>
#include <QGridLayout>
#include <QPainter>
#include <qscreen.h>
#include <QApplication>
#include <QResizeEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class Toastr; }
QT_END_NAMESPACE


class EToastr : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(float popupOpacity READ getPopupOpacity WRITE setPopupOpacity)
    Q_PROPERTY(float timelapse READ getTimelapse WRITE setTimelapse)

    void setPopupOpacity(float opacity);
    float getPopupOpacity() const;    
    

    void setTimelapse(float timelapse);
    float getTimelapse() const;

    EWidget* _parent = nullptr;
public:

   

    EToastr(EWidget *parent = nullptr, bool showIcon = true); // if parent is null, Desktop will be the parent.
    ~EToastr();

    enum TOASTR_STYLE {
        MESSAGE,
        SUCCESS,
        WARNING,
        INFO,
        FAIL
    };

    enum TOASTR_DIRECTION {        
        RIGHT_TO_LEFT,
        TOP_TO_BOTTOM,
        BOTTOM_TO_TOP,        
        LEFT_TO_RIGHT
    };

    void setText(QString text);
    void setDuration(uint msec);
    void setShowDuration(uint msec);
    void setHideDuration(uint msec);
    void setStyle(TOASTR_STYLE style);
    void setIcon(QPixmap pixmap);
    void setBackgroundColor(QString color);
    void setOpacity(uint opacity);
    void setTextColor(QString color);

    
    QRect getParentGeometry() const;
    int   getCurrentIndex()   const;
    bool  parentIsDesktop()   const;

    void  moveCustom(QPoint rect, bool animated = true);
    void  moveanimated(int x, int y, bool animated = true) { moveCustom(QPoint(x, y), animated); }
    void  Quit();

    QPropertyAnimation* createAnimation(QPoint pointStart, QPoint pointEnd);
private:
    void calculateStartXY(QRect rect, TOASTR_DIRECTION direction, QRect &out_rect);
    void updateLocations();
    void All_updateLocations(); // For update locatinos of all Toastr widgets


    void deleteFromList(EToastr* target);
public slots:
    void show(TOASTR_DIRECTION direction);
    void SlotParentResized(QResizeEvent *event);
    void SlotParentMoved(QMoveEvent *event);

private slots:
    void hideAnimation();
    void hide();

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    bool m_closing = false; // When toastr hiding, if user resize or move parent widget it will cause a bug, for fix that. (Toastr tam kapanacakken parent boyutu yada konumu deðiþince önceki animasyon ile çakýþýyor) bu bool sayesinde 2. animasyon ignore edilecek eðer kapanýyorsa
    Ui::Toastr* ui;

    static uint TIMEOUT;
    static uint COUNT_TOASTR;
    static QVector<EToastr*> vectorToastr;
    

    QTimer *timer;
    uint showTimeout = 150;
    uint hideTimeout = 500;
    uint animateTimeout = 350;

    TOASTR_DIRECTION directionShow = TOASTR_DIRECTION::RIGHT_TO_LEFT;
    TOASTR_STYLE currentStyle;
    QPropertyAnimation animationOpacity;
    QPropertyAnimation* animationSlide = nullptr;
    float popupOpacity=1.0f;
    float timeLapse=1.0f;

    QColor backgroundColor = Qt::gray;
    QString textColor = "#000000";
    uint opacity = 200;
    QColor GREEN = QColor(117,182,117,200);
    QColor RED = QColor(203,94,89,200);
    QColor YELLOW = QColor(249,171,53,200);
    QColor BLUE = QColor(88,173,196,200);
    QColor BLACK = QColor(0,0,0,200);

    const static uchar ROUND_RECT{15};
    const static uchar MARGIN_WIDTH{36};
    const static uchar SPACE_BETWEEN{10};

    bool m_iconVisible = true;

protected:
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual QSize sizeHint() const;
};


#endif // TOASTR_H
