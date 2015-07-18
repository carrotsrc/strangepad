#ifndef __SVINDICATOR_HPP_1437142126__
#define __SVINDICATOR_HPP_1437142126__
#include <QWidget>
#include <QVector>

class SVIndicator : public QWidget {
Q_OBJECT
public:
	enum Orientation {
		Right,Left
	};

	SVIndicator(QWidget* parent = 0);
	~SVIndicator();
	void paintEvent(QPaintEvent*);

	void setOrientation(SVIndicator::Orientation orientation);

signals:
	public slots:

private:
	QVector<QRectF> mLeds;
	QColor *high, *mid, *low;
	SVIndicator::Orientation mOrientation;
};
#endif

