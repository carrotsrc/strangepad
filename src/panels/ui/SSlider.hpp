#ifndef __SSLIDER_HPP_1437163102__
#define __SSLIDER_HPP_1437163102__
 
#include <QAbstractSlider>
#include <QVector>
#include <QVBoxLayout>
#include <QLabel>

class SSlider : public QAbstractSlider {
Q_OBJECT
public:
	enum Orientation {
		Horizontal,Vertical
	};

	enum Width {
		Small,Medium,Large
	};

	SSlider(QWidget* parent = 0);


	void paintEvent(QPaintEvent*);
	void mousePressEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);

	void setOrientation(SSlider::Orientation orientation);
	void setBarSize(SSlider::Width width);
	QSize sizeHint() const;

signals:
	public slots:

private:
	SSlider::Orientation mOrientation;
	short mWidth, mHalfWidth;
	
	SSlider::Width mSize;
	QRectF *cursor;
	QLabel mValueLabel;
	QVBoxLayout mContainer;
	bool mGrabbed;
};
#endif

