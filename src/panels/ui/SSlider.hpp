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
	~SSlider();
	void paintEvent(QPaintEvent*);

	void setOrientation(SSlider::Orientation orientation);

signals:
	public slots:

private:
	SSlider::Orientation mOrientation;
};
#endif

