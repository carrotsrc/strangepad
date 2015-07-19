#ifndef __SWAVEFORM_HPP_1437342283__
#define __SWAVEFORM_HPP_1437342283__
#include <QWidget>

class SWaveform : public QWidget {
Q_OBJECT
public:
	explicit SWaveform(QWidget *parent = 0);

	void setData(const short* data);
	void paintEvent(QPaintEvent *);
signals:
	public slots:

private:
	short* waveData;
};
#endif

