#ifndef __WAVEFORM_HPP_1437388311__
#define __WAVEFORM_HPP_1437388311__

#include <memory>
#include <QImage>
#include <QString>

#include "leveldb/db.h"
#include "Waveform.hpp"

class WaveformManager {
public:
	enum Stat { MaxSize = 1572864 };

	void regenerate(Waveform *waveform);
	std::unique_ptr<Waveform> generate(int width, int height, const signed short *raw, unsigned long long spc);

private:

	signed short* storeCompress(const signed short *raw, unsigned long long spc);
	QPixmap compress(int width, int height, const signed short *compressed);
	QString hash(const signed short *raw, unsigned long long spc);
};
#endif

