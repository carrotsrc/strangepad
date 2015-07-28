#ifndef __SVINDICATOR_HPP_1437142126__
#define __SVINDICATOR_HPP_1437142126__
#include <QWidget>
#include <QVector>
#include <QMutex>

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
	void setValue(float value);

signals:
	public slots:

private:
	QVector<QRectF> mLeds;
	QString highOn, midOn, lowOn,
	       highOff, midOff, lowOff;

	int mHeightTracker;
	float mValue;
	SVIndicator::Orientation mOrientation;
	QMutex mMut;
};
#endif

