#ifndef __SVINDICATOR_HPP_1437142126__
#define __SVINDICATOR_HPP_1437142126__
#include <QWidget>
#include <QVector>
#include <QMutex>

class SLevel : public QWidget {
Q_OBJECT
public:
	enum Orientation {
		Right,Left
	};

	SLevel(QWidget* parent = 0);
	~SLevel();
	void paintEvent(QPaintEvent*);

	void setOrientation(SLevel::Orientation orientation);
	void setValue(float value);
	QSize sizeHint() const;
signals:
	public slots:

private:
	QVector<QRectF> mLeds;
	QString highOn, midOn, lowOn,
	       highOff, midOff, lowOff;

	int mHeightTracker, mWidthTracker;
	float mValue;
	SLevel::Orientation mOrientation;
	QMutex mMut;
};
#endif

