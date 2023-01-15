#include "EToastr.h"
#include "ui_toastr.h"
#include <QDebug>

uint EToastr::TIMEOUT = 2000;
uint EToastr::COUNT_TOASTR = 0;
QVector<EToastr*> EToastr::vectorToastr;

void EToastr::setPopupOpacity(float opacity)
{
	popupOpacity = opacity;
	setWindowOpacity(opacity);
}

float EToastr::getPopupOpacity() const { return popupOpacity; }

void EToastr::setTimelapse(float timelapse)
{
	this->timeLapse = timelapse;
	if (timelapse == 0.0f)
		setPopupOpacity(0.85f); // set opacity when toastr slides (hiding)
	else
		setPopupOpacity(1.0f);
}

float EToastr::getTimelapse() const { return this->timeLapse; }

QRect EToastr::getParentGeometry() const
{
	if (parentIsDesktop())// is parent null (it means parent is Desktop)
	{
        QScreen* screen = QGuiApplication::primaryScreen();
		QRect geometry = screen->geometry();
		return geometry;
		//return QApplication::desktop()->geometry();
	}
	else {
		//_parent->adjustSize();
		//qDebug() << _parent->geometry();
		auto geo = _parent->geometry();
		return geo;
	}
}
void EToastr::SlotParentResized(QResizeEvent* event) {
	Q_UNUSED(event);
	updateLocations();
}
void EToastr::SlotParentMoved(QMoveEvent* event)
{
	Q_UNUSED(event);
	updateLocations();
}
void EToastr::Quit()
{
	QWidget::hide();
	COUNT_TOASTR--;
	deleteFromList(this);
	All_updateLocations();
	delete this;
}
bool EToastr::parentIsDesktop() const { return (this->_parent == nullptr); }

void EToastr::calculateStartXY(QRect rect, TOASTR_DIRECTION direction, QRect& out_rect)
{
	out_rect = rect;
	const QRect targetGeometry = getParentGeometry();	
	const int screen_w = targetGeometry.width();
	const int screen_h = targetGeometry.height();


    const int save_h = out_rect.height(); // when we use setY function, height too changes so we need to save it first then change Y coordinate and restore back height at the end. / setY kullanilinca height de degisiyor bunu cozmek icin height once bir degiskende sakliyorum, setY kullanildiktan sonra tekrar bu degiskendeki degeri yapiyorum

	if (direction == TOASTR_DIRECTION::RIGHT_TO_LEFT) { // SAGDAN GELECEK
        out_rect.setX(screen_w + MARGIN_WIDTH);
		out_rect.setY(MARGIN_WIDTH);
	}
	else if (direction == TOASTR_DIRECTION::LEFT_TO_RIGHT) { // SOLDAN GELECEK
        out_rect.setX(-MARGIN_WIDTH);
        out_rect.setY(MARGIN_WIDTH);
	}
    else if (direction == TOASTR_DIRECTION::TOP_TO_BOTTOM) { // YUKARDAN GELECEK
		out_rect.setX((screen_w - width())/2);
        out_rect.setY(-MARGIN_WIDTH);
	}
    else if (direction == TOASTR_DIRECTION::BOTTOM_TO_TOP) { // ASAGIDAN GELECEK
        out_rect.setX((screen_w - width())/2);
        out_rect.setY(screen_h+MARGIN_WIDTH);
	}
    out_rect.setHeight(save_h); // if we don't do that, height will return minmumSize always (30)
}
void EToastr::updateLocations()
{
	if (m_closing) // Animasyon kayarak kapanacakken parent resize/move olursa bunlar çakışıyor bunu önlemek için
		return;
	const QRect targetGeometry = getParentGeometry();
    const int screen_x = targetGeometry.x(); // for multiple monitors

	const int screen_w = targetGeometry.width();
	const int screen_h = targetGeometry.height();

    const int START_Y = MARGIN_WIDTH + (this->getCurrentIndex() * (this->height() + SPACE_BETWEEN));

	switch (directionShow) {
        case TOASTR_DIRECTION::RIGHT_TO_LEFT:
        {
            moveanimated(screen_w - MARGIN_WIDTH - width(),
                START_Y);
        } break;
        case TOASTR_DIRECTION::LEFT_TO_RIGHT:
        {
            moveanimated(MARGIN_WIDTH,
                START_Y);
        }break;
        case TOASTR_DIRECTION::BOTTOM_TO_TOP:
        {
            moveanimated((screen_w - width())/2,
                screen_h - (START_Y + height()));
        }break;
        case TOASTR_DIRECTION::TOP_TO_BOTTOM:
        {
            moveanimated((screen_w - width())/ 2,
                START_Y);
        }break;
	}
}

void EToastr::All_updateLocations()
{
	for (int i = 0; i < vectorToastr.length(); i++) {
        auto data = vectorToastr.at(i);
		data->updateLocations();
	}
}

void EToastr::deleteFromList(EToastr* target)
{
	const int myIndex = vectorToastr.indexOf(target);
	if (myIndex > -1 && myIndex < vectorToastr.count())
		vectorToastr.remove(myIndex);
}


int EToastr::getCurrentIndex() const
{	
	auto it = std::find(this->vectorToastr.begin(), this->vectorToastr.end(), this);
	if (it != this->vectorToastr.end())
		return (int)(it - this->vectorToastr.begin());

	return -1;
}

EToastr::EToastr(EWidget* parent, bool showIcon)
	: QFrame(parent)
	, ui(new Ui::Toastr)
{
	ui->setupUi(this);
	_parent = parent;
	setParent(_parent);
    if (parent) {
		QObject::connect(_parent, &EWidget::resizeEvent, this, &EToastr::SlotParentResized);
		QObject::connect(_parent, &EWidget::moveEvent, this, &EToastr::SlotParentMoved);
	}
	this->setAttribute(Qt::WA_Hover, true);
	setWindowFlags(Qt::FramelessWindowHint |
		(parentIsDesktop() ? Qt::Tool : Qt::Widget) |
		(parentIsDesktop() ? Qt::WindowStaysOnTopHint : Qt::Widget));
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_ShowWithoutActivating);

	ui->textLabel->setStyleSheet(QString("QLabel {font: 600 10pt 'Segoe UI Semibold'; color: %1}").arg(textColor));
    m_iconVisible = showIcon; ui->iconLabel->setVisible(showIcon);

	animationOpacity.setTargetObject(this);
	//animationOpacity.setPropertyName("popupOpacity");
	animationOpacity.setPropertyName("timelapse");
	connect(&animationOpacity, &QAbstractAnimation::finished, this, &EToastr::hide);



	timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &EToastr::hideAnimation);
}

EToastr::~EToastr() { delete ui; }

void EToastr::moveCustom(QPoint tarPoint, bool animated)
{
    //qDebug() << "moveCustom: " << rect;
    if (!animated)
		move(tarPoint);
	else {
		QPoint curPoint = pos();
		createAnimation(curPoint, tarPoint);
	}
}

QPropertyAnimation* EToastr::createAnimation(QPoint pointStart, QPoint pointEnd) {	
	QPropertyAnimation* anim = new QPropertyAnimation(this, "pos");
	anim->setStartValue(pointStart);
	anim->setEndValue(pointEnd);
	anim->setDuration(animateTimeout);
	anim->setEasingCurve(QEasingCurve::OutCubic);
	anim->start(QPropertyAnimation::DeleteWhenStopped);
	return anim;
}

void EToastr::setText(QString text) { ui->textLabel->setText(text); }

void EToastr::setDuration(uint msec) { TIMEOUT = msec; }

void EToastr::setShowDuration(uint msec) { this->showTimeout = msec; }

void EToastr::setHideDuration(uint msec) { this->hideTimeout = msec; }

void EToastr::setStyle(TOASTR_STYLE style)
{
#ifdef __MINGW32__ // Qt Creator
    QString imgpath = ":/img/";
#else // visual studio
	QString imgpath = "Ui/img/";
#endif
	switch (style) {
	case TOASTR_STYLE::MESSAGE: {
		backgroundColor = BLACK;
		setTextColor("#EFEFEF");
		backgroundColor.setAlpha(this->opacity);
        QPixmap icon = QPixmap(imgpath+"message.png");
		ui->iconLabel->setPixmap(icon);
	} break;
	case TOASTR_STYLE::SUCCESS: {
		backgroundColor = GREEN;
		setTextColor("#121212");
		backgroundColor.setAlpha(this->opacity);
        QPixmap icon = QPixmap(imgpath+"check.png");
		ui->iconLabel->setPixmap(icon);
	} break;
	case TOASTR_STYLE::WARNING: {
		backgroundColor = YELLOW;
		backgroundColor.setAlpha(this->opacity);
		QPixmap icon = QPixmap(imgpath+"warning.png");
		ui->iconLabel->setPixmap(icon);
	} break;
	case TOASTR_STYLE::INFO: {
		backgroundColor = BLUE;
		backgroundColor.setAlpha(this->opacity);
		setTextColor("#EDEDED");
        QPixmap icon = QPixmap(imgpath+"info.png");
		ui->iconLabel->setPixmap(icon);
	} break;
	case TOASTR_STYLE::FAIL: {
		backgroundColor = RED;
		backgroundColor.setAlpha(this->opacity);
		setTextColor("#EFEFEF");
        QPixmap icon = QPixmap(imgpath+"shield.png");
		ui->iconLabel->setPixmap(icon);
	} break;
	}
}

void EToastr::setIcon(QPixmap pixmap)
{
	ui->iconLabel->show();
    ui->iconLabel->setPixmap(pixmap);
}

void EToastr::setBackgroundColor(QString color) { this->backgroundColor = color; }

void EToastr::setOpacity(uint opacity)
{
	this->backgroundColor.setAlpha(opacity);
	this->opacity = opacity;
}

void EToastr::setTextColor(QString color) { ui->textLabel->setStyleSheet(QString("QLabel {font: 600 10pt 'Segoe UI Semibold'; color: %1}").arg(color)); }

void EToastr::hide()
{
	//if (getPopupOpacity() == 0.0) {
	if (getTimelapse() == 0.0){
		QPoint pointCurrent = pos();
		QPoint pointEnd = pos();
        int deltaX = 0, deltaY = 0;

		if (directionShow == TOASTR_DIRECTION::RIGHT_TO_LEFT)
			deltaX =  (width() + MARGIN_WIDTH);
		else if (directionShow == TOASTR_DIRECTION::LEFT_TO_RIGHT)
			deltaX = -(width() + MARGIN_WIDTH);
		else if (directionShow == TOASTR_DIRECTION::BOTTOM_TO_TOP)
            deltaY =  (height() + MARGIN_WIDTH);
		else if (directionShow == TOASTR_DIRECTION::TOP_TO_BOTTOM)
			deltaY = -(height() + MARGIN_WIDTH);

		

		pointEnd.setX(pointEnd.x() + deltaX);
		pointEnd.setY(pointEnd.y() + deltaY);

		//rectEnd.setWidth(geoCurrent.width()); // when rectEnd.x changes, rectEnd.width too changes. we don't want that.
		//rectEnd.setHeight(geoCurrent.height());// when rectEnd.y changes, rectEnd.height too changes. we don't want that.
		animationSlide = createAnimation(pointCurrent, pointEnd);

		m_closing = true; // Fix for Animation combine bug (2 animations at the same time)
	
		if(true)connect(animationSlide, &QPropertyAnimation::finished, 
			[this]() {
                this->Quit();
		});
	}
}

void EToastr::show(TOASTR_DIRECTION direction)
{
	directionShow = direction;
	vectorToastr.append(this);
	COUNT_TOASTR++;
    adjustSize();

	QRect curRect = geometry(); QRect outRect;
	calculateStartXY(curRect, direction, outRect);
    setGeometry(outRect);


	//setWindowOpacity(0.0); 

	animationOpacity.setDuration(this->showTimeout);
	animationOpacity.setStartValue(0.0);
	animationOpacity.setEndValue(1.0);

	updateLocations();

	QWidget::show();
    QObject::connect(&animationOpacity, &QPropertyAnimation::valueChanged, [this]() {
        this->update();
    });

	animationOpacity.start();
	timer->start(TIMEOUT);
}

void EToastr::hideAnimation()
{
	if (TIMEOUT == 0)
		return;
	timer->stop();
	animationOpacity.setDuration(this->hideTimeout);
	animationOpacity.setStartValue(1.0f);
	animationOpacity.setEndValue(0.0f);
	animationOpacity.start();
}

void EToastr::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);
	QPainter paint;
	paint.begin(this);
	paint.setRenderHints(QPainter::Antialiasing);
	paint.setBrush(QBrush(this->backgroundColor));
	paint.setPen(Qt::NoPen);
	auto calcRect = this->rect();
	/*calcRect.setX((calcRect.width() * animation.currentValue().toFloat()));*/
	paint.drawRoundedRect(calcRect, ROUND_RECT, ROUND_RECT);
	paint.end();   
}

void EToastr::enterEvent(QEvent* event)
{
	timer->stop();
	this->backgroundColor.setAlpha(255);
	QWidget::enterEvent(event);
}

void EToastr::leaveEvent(QEvent* event)
{
	timer->start();
	this->backgroundColor.setAlpha(opacity);
	QWidget::leaveEvent(event);
}

void EToastr::mousePressEvent(QMouseEvent* event)
{
	Q_UNUSED(event);
	Quit();	
}
QSize EToastr::sizeHint() const{

    QSize result{maximumWidth(), MARGIN_WIDTH};

    if (parentIsDesktop())
        return result;

    auto cm = layout()->contentsMargins();
    //result += QSize(0, cm.top() + cm.bottom());


    ui->textLabel->adjustSize();
    ui->iconLabel->adjustSize();
    QSize size_label_text = ui->textLabel->size();
    QSize size_label_icon = ui->iconLabel->size();
    QFontMetrics fm(ui->textLabel->font());
    int availableWidth = maximumWidth();
    if (m_iconVisible)
        availableWidth -= ui->iconLabel->width();

    // For calculate text height / Yazi uzun oldugunda asagi dogru widgetin uzamasi icin
    QRect text_rect = fm.boundingRect(QRect(0, 0,
                                           availableWidth - cm.top() - cm.bottom(), 0),
                                     Qt::TextDontClip | Qt::TextWordWrap, ui->textLabel->text());


    result+= QSize(0, std::max(text_rect.height(), ui->iconLabel->height()));
    //return QSize(200,200); // test
    return result;
}
