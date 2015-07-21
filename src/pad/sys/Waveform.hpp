#ifndef __WAVEFORM_HPP_1437388311__
#define __WAVEFORM_HPP_1437388311__
#include <QImage>
#include <QString>
class Waveform {
public:
	enum Stat { MaxSize = 1572864 };
	void setRaw(signed short *raw, long long samples);

	QString hash();

	void minCompression();
	void recompress();
	QImage generate(int width, int height);

private:
	signed short *mRaw, *mCompressed;
	unsigned long long mLenR;
};
#endif

