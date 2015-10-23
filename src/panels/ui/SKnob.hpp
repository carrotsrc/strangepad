#ifndef __SKNOB_HPP_1437320882__
#define __SKNOB_HPP_1437320882__

#include <QAbstractSlider>

class SKnob : public QAbstractSlider {
Q_OBJECT
public:
	SKnob(QWidget* parent = 0);

	void paintEvent(QPaintEvent*);
	void mousePressEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
signals:
	public slots:

private:
	QRectF *mKnobContainer;
	QRectF *mKnobHighlight;
	int mOldY;
	bool mGrabbed;
};
#endif

