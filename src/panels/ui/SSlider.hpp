#ifndef __SSLIDER_HPP_1437163102__
#define __SSLIDER_HPP_1437163102__
 
#include <QAbstractSlider>
#include <QVector>

class SSlider : public QAbstractSlider {
Q_OBJECT
public:
	enum Orientation {
		Horizontal,Vertical
	};

	SSlider(QWidget* parent = 0);
	void paintEvent(QPaintEvent*);
	void mousePressEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);

	void setOrientation(SSlider::Orientation orientation);

signals:
	public slots:

private:
	SSlider::Orientation mOrientation;
	QRectF *cursor;
	bool mGrabbed;
};
#endif

