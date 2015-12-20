#ifndef __WAVEFORM_HPP_1437388311__
#define __WAVEFORM_HPP_1437388311__

#include <memory>
#include <QImage>
#include <QString>

#include "Waveform.hpp"
using pcm_sample = float;

class WaveformManager {
public:
	enum Stat { MaxSize = 1572864 };

	void regenerate(Waveform *waveform);
	std::unique_ptr<Waveform> generate(int width, int height, const pcm_sample *raw, unsigned long long spc);
	std::unique_ptr<Waveform> generate(int width, int height, const pcm_sample *raw, unsigned long long spc, QString hashValue);
	QString hash(const pcm_sample *raw, unsigned long long spc);

private:
	pcm_sample* storeCompress(const pcm_sample *raw, unsigned long long spc, unsigned int *blockSize);
	QPixmap compress(int width, int height, const pcm_sample *compressed, unsigned int *blockSize);
	
};

struct WaveStore {
	int blockSize;
	pcm_sample waveform[WaveformManager::MaxSize];
};

#endif

