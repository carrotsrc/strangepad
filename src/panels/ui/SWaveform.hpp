#ifndef __SWAVEFORM_HPP_1437342283__
#define __SWAVEFORM_HPP_1437342283__
#include <QWidget>

class SWaveform : public QWidget {
Q_OBJECT
public:
	explicit SWaveform(QWidget *parent = 0);

	void setWaveData(const short* data, long long length);
	void generateWaveform();
	void paintEvent(QPaintEvent *);

	void mouseMoveEvent(QMouseEvent*);

signals:
	public slots:

private:
	signed short* mWaveData;
	int mWaveLength, mSampleStep, mHoverPosition;

	QRectF mWaveRect;

};
#endif

