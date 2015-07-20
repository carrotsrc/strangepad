#ifndef __WAVEFORM_HPP_1437388311__
#define __WAVEFORM_HPP_1437388311__
#include <QImage>
#include <QString>
class Waveform {
public:
	enum Stat { StoreSize = 1572864 };
	void setRaw(signed short *raw, long long samples);
	QString hash();

	void minCompression();

private:
	signed short *mRaw, *mMin;
	unsigned long long mLenR;
};
#endif

