#ifndef __SWAVEFORM_HPP_1437342283__
#define __SWAVEFORM_HPP_1437342283__
#include <QWidget>
#include "../../pad/sys/WaveformManager.hpp"
class SWaveform : public QWidget {
Q_OBJECT
public:
	explicit SWaveform(QWidget *parent = 0);

	void setWaveData(const float* data, long long length);
	void generateWaveform();
	void toggleBgHighlight(bool flag);
	void paintEvent(QPaintEvent *);

	void mouseMoveEvent(QMouseEvent*);
signals:
	public slots:

private:
	float* mWaveData;
	int mWaveLength, mSampleStep, mHoverPosition;

	std::unique_ptr<Waveform> mWaveform;

	QRectF mWaveRect;

	bool isLoaded, mBgHighlight;
};
#endif

